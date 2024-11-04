/// Denom.org
///
/// MIT No Attribution.
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software
/// without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
/// permit persons to whom the Software is furnished to do so.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
/// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
/// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
///
/// Author:  Sergey Novochenko,  Digrol@gmail.com

#include "stdinc.h"

#include "utils.h"
#include "exception.h"

using std::string;
using std::wstring;
using std::list;
using Denom::ExStackInfo;

#ifdef _WIN32
#include <windows.h>

namespace {

#ifdef _DEBUG

// =====================================================================================================================
/// Unwinding call stack.

#pragma pack(push,8)
// Shutoff warning in dbghelp.h
#pragma warning(push)
#pragma warning(disable : 4091)
#include <dbghelp.h>
#pragma warning(pop)
#pragma pack(pop)

// max name length for found symbols
const int STACKWALK_MAX_NAMELEN = 1024;

// ---------------------------------------------------------------------------------------------------------------------
BOOL __stdcall myReadProcMem( HANDLE hProcess, DWORD64 qwBaseAddress, PVOID lpBuffer, DWORD nSize, LPDWORD lpNumberOfBytesRead )
{
	SIZE_T st;
	BOOL bRet = ReadProcessMemory(hProcess, (LPVOID) qwBaseAddress, lpBuffer, nSize, &st);
	*lpNumberOfBytesRead = (DWORD)st;
	return bRet;
}

// ---------------------------------------------------------------------------------------------------------------------
struct CallstackEntry
{
	DWORD64 offset;  // if 0, we have no valid entry
	CHAR name[ STACKWALK_MAX_NAMELEN ];
	CHAR undName[ STACKWALK_MAX_NAMELEN ];
	CHAR undFullName[ STACKWALK_MAX_NAMELEN ];
	DWORD lineNumber;
	CHAR lineFileName[ STACKWALK_MAX_NAMELEN ];
};

// ---------------------------------------------------------------------------------------------------------------------
/// ToolHelp32
#define TH32CS_SNAPMODULE   0x00000008
#pragma pack( push, 8 )
struct MODULEENTRY32
{
	DWORD   dwSize;
	DWORD   th32ModuleID;       // This module
	DWORD   th32ProcessID;      // owning process
	DWORD   GlblcntUsage;       // Global usage count on the module
	DWORD   ProccntUsage;       // Module usage count in th32ProcessID's context
	BYTE  * modBaseAddr;        // Base address of module in th32ProcessID's context
	DWORD   modBaseSize;        // Size in bytes of module starting at modBaseAddr
	HMODULE hModule;            // The hModule of this module in th32ProcessID's context
	char    szModule[ 256 ];
	char    szExePath[ MAX_PATH ];
};
typedef MODULEENTRY32* LPMODULEENTRY32;
#pragma pack( pop )

// =====================================================================================================================
class StackWalker
{
public:
	void WriteStack( std::list<ExStackInfo>* output_list );

	StackWalker();
	~StackWalker();

private:
	bool m_inited;
	HMODULE m_hDbhHelp;

	list<ExStackInfo>* m_output_list; // All info about stack

	DWORD LoadModule( HANDLE hProcess, LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size );
	void LoadModules();
	void OnCallstackEntry( CallstackEntry& entry );


	typedef BOOL (__stdcall *tSymCleanup)( IN HANDLE hProcess );
	tSymCleanup pSymCleanup;

	// SymFunctionTableAccess64()
	typedef PVOID (__stdcall *tSFTA)( HANDLE hProcess, DWORD64 AddrBase );
	tSFTA pSFTA;

	// SymGetLineFromAddr64()
	typedef BOOL (__stdcall *tSGLFA)( IN HANDLE hProcess, IN DWORD64 dwAddr,
		OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE64 Line );
	tSGLFA pSGLFA;

	// SymGetModuleBase64()
	typedef DWORD64 (__stdcall *tSGMB)( IN HANDLE hProcess, IN DWORD64 dwAddr );
	tSGMB pSGMB;

	// SymGetOptions()
	typedef DWORD (__stdcall *tSGO)( VOID );
	tSGO pSGO;

	// SymGetSymFromAddr64()
	typedef BOOL (__stdcall *tSGSFA)( IN HANDLE hProcess, IN DWORD64 dwAddr,
		OUT PDWORD64 pdwDisplacement, OUT PIMAGEHLP_SYMBOL64 Symbol );
	tSGSFA pSGSFA;

	// SymLoadModule64()
	typedef DWORD64 (__stdcall *tSLM)( IN HANDLE hProcess, IN HANDLE hFile,
		IN PSTR ImageName, IN PSTR ModuleName, IN DWORD64 BaseOfDll, IN DWORD SizeOfDll );
	tSLM pSLM;

	typedef DWORD (__stdcall *tSymSetOptions)( IN DWORD SymOptions );
	tSymSetOptions pSymSetOptions;

	// StackWalk64()
	typedef BOOL (__stdcall *tStackWalk64)( DWORD MachineType, HANDLE hProcess, HANDLE hThread, 
		LPSTACKFRAME64 StackFrame, PVOID ContextRecord,
		PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine,
		PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine,
		PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine,
		PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress );
	tStackWalk64 pStackWalk64;

	// UnDecorateSymbolName()
	typedef DWORD (__stdcall WINAPI *tUDSN)( PCSTR DecoratedName, PSTR UnDecoratedName,
		DWORD UndecoratedLength, DWORD Flags );
	tUDSN pUDSN;
};

// ---------------------------------------------------------------------------------------------------------------------
StackWalker::StackWalker()
{
	m_output_list = NULL;
	m_hDbhHelp = NULL;
	pSymCleanup = NULL;
	pSFTA = NULL;
	pSGLFA = NULL;
	pSGMB = NULL;
	pSGO = NULL;
	pSGSFA = NULL;
	pSLM = NULL;
	pSymSetOptions = NULL;
	pStackWalk64 = NULL;
	pUDSN = NULL;

	m_inited = false;

	m_hDbhHelp = LoadLibraryA( "dbghelp.dll" );
	if( m_hDbhHelp == NULL )
		return;

	typedef BOOL (__stdcall *tSymInitialize)( IN HANDLE hProcess, IN PSTR UserSearchPath, IN BOOL fInvadeProcess );
	tSymInitialize pSymInitialize = (tSymInitialize) GetProcAddress(m_hDbhHelp, "SymInitialize" );

	pSymCleanup = (tSymCleanup) GetProcAddress(m_hDbhHelp, "SymCleanup" );

	pStackWalk64 = (tStackWalk64) GetProcAddress(m_hDbhHelp, "StackWalk64" );
	pSGO = (tSGO) GetProcAddress(m_hDbhHelp, "SymGetOptions" );
	pSymSetOptions = (tSymSetOptions) GetProcAddress(m_hDbhHelp, "SymSetOptions" );

	pSFTA = (tSFTA) GetProcAddress(m_hDbhHelp, "SymFunctionTableAccess64" );
	pSGLFA = (tSGLFA) GetProcAddress(m_hDbhHelp, "SymGetLineFromAddr64" );
	pSGMB = (tSGMB) GetProcAddress(m_hDbhHelp, "SymGetModuleBase64" );

	pSGSFA = (tSGSFA) GetProcAddress(m_hDbhHelp, "SymGetSymFromAddr64" );
	pUDSN = (tUDSN) GetProcAddress(m_hDbhHelp, "UnDecorateSymbolName" );
	pSLM = (tSLM) GetProcAddress(m_hDbhHelp, "SymLoadModule64" );

	if ( pSymInitialize == NULL || pSymCleanup == NULL || pSymSetOptions == NULL || pStackWalk64 == NULL
		|| pSFTA == NULL || pSGMB == NULL || pSGO == NULL
		|| pSGSFA == NULL || pUDSN == NULL || pSLM == NULL )
	{
		FreeLibrary( m_hDbhHelp );
		m_hDbhHelp = NULL;
		pSymCleanup = NULL;
		return;
	}

	// SymInitialize
	if( pSymInitialize( GetCurrentProcess(), NULL, FALSE ) == FALSE )
		return;

	DWORD symOptions = pSGO();  // SymGetOptions
	symOptions |= SYMOPT_LOAD_LINES;
	symOptions |= SYMOPT_FAIL_CRITICAL_ERRORS;
	pSymSetOptions(symOptions);

	m_inited = true;

	LoadModules();
}

// ---------------------------------------------------------------------------------------------------------------------
StackWalker::~StackWalker()
{
	if( pSymCleanup != NULL )
		pSymCleanup( GetCurrentProcess() );  // SymCleanup

	if( m_hDbhHelp != NULL)
		FreeLibrary( m_hDbhHelp );
}

// ---------------------------------------------------------------------------------------------------------------------
DWORD StackWalker::LoadModule( HANDLE hProcess, LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size )
{
	CHAR *szImg = _strdup( img );
	CHAR *szMod = _strdup( mod );
	DWORD result = ERROR_SUCCESS;

	if( pSLM( hProcess, 0, szImg, szMod, baseAddr, size ) == 0 )
		result = GetLastError();

	if (szImg != NULL) free(szImg);
	if (szMod != NULL) free(szMod);
	return result;
}

// ---------------------------------------------------------------------------------------------------------------------
void StackWalker::LoadModules()
{
	if( !m_inited )
		return;

	m_inited = false;

	HANDLE hProcess = GetCurrentProcess();
	DWORD dwProcessId = GetCurrentProcessId();

	// CreateToolhelp32Snapshot()
	typedef HANDLE (__stdcall *tCT32S)( DWORD dwFlags, DWORD th32ProcessID );
	// Module32First()
	typedef BOOL (__stdcall *tM32F)( HANDLE hSnapshot, LPMODULEENTRY32 lpme);
	// Module32Next()
	typedef BOOL (__stdcall *tM32N)( HANDLE hSnapshot, LPMODULEENTRY32 lpme );

	// try both dlls...
	const CHAR *dllname[] = { "kernel32.dll", "tlhelp32.dll" };
	HINSTANCE hToolhelp = NULL;
	tCT32S pCT32S = NULL;
	tM32F pM32F = NULL;
	tM32N pM32N = NULL;

	size_t i;

	for( i = 0; i < (sizeof(dllname) / sizeof(dllname[0])); i++ )
	{
		hToolhelp = LoadLibraryA( dllname[i] );
		if( hToolhelp == NULL )
			continue;
		pCT32S = (tCT32S) GetProcAddress(hToolhelp, "CreateToolhelp32Snapshot");
		pM32F = (tM32F) GetProcAddress(hToolhelp, "Module32First");
		pM32N = (tM32N) GetProcAddress(hToolhelp, "Module32Next");
		if ( (pCT32S != NULL) && (pM32F != NULL) && (pM32N != NULL) )
			break; // found the functions!
		FreeLibrary(hToolhelp);
		hToolhelp = NULL;
	}

	if( hToolhelp == NULL )
		return;

	HANDLE hSnap = pCT32S( TH32CS_SNAPMODULE, dwProcessId );
	if (hSnap == (HANDLE) -1)
	{
		FreeLibrary(hToolhelp);
		return;
	}

	MODULEENTRY32 me;
	me.dwSize = sizeof(me);
	BOOL keepGoing = pM32F( hSnap, &me );

	int cnt = 0;
	while( keepGoing )
	{
		this->LoadModule(hProcess, me.szExePath, me.szModule, (DWORD64) me.modBaseAddr, me.modBaseSize);
		cnt++;
		keepGoing = pM32N( hSnap, &me );
	}

	CloseHandle( hSnap );
	FreeLibrary( hToolhelp );

	if( cnt > 0 )
	{
		m_inited = true;
	}
}

// ---------------------------------------------------------------------------------------------------------------------
/// @param output_list - Write here all info about stack.
void StackWalker::WriteStack( std::list<ExStackInfo>* output_list )
{
	output_list->clear();
	
	if( !m_inited )
		return;

	m_output_list = output_list;

	HANDLE hProcess = GetCurrentProcess();

	CONTEXT c;
	memset( &c, 0, sizeof(CONTEXT) );
	c.ContextFlags = CONTEXT_FULL;
	RtlCaptureContext( &c );

	// init STACKFRAME for first call
	STACKFRAME64 s;
	memset( &s, 0, sizeof(s) );
	DWORD imageType;

#ifdef _M_IX86
	// normally, call ImageNtHeader() and use machine info from PE header
	imageType = IMAGE_FILE_MACHINE_I386;
	s.AddrPC.Offset = c.Eip;
	s.AddrPC.Mode = AddrModeFlat;
	s.AddrFrame.Offset = c.Ebp;
	s.AddrFrame.Mode = AddrModeFlat;
	s.AddrStack.Offset = c.Esp;
	s.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
	imageType = IMAGE_FILE_MACHINE_AMD64;
	s.AddrPC.Offset = c.Rip;
	s.AddrPC.Mode = AddrModeFlat;
	s.AddrFrame.Offset = c.Rsp;
	s.AddrFrame.Mode = AddrModeFlat;
	s.AddrStack.Offset = c.Rsp;
	s.AddrStack.Mode = AddrModeFlat;
#else
	#error "Platform not supported!"
#endif

	IMAGEHLP_SYMBOL64 *pSym = (IMAGEHLP_SYMBOL64 *) malloc( sizeof(IMAGEHLP_SYMBOL64) + STACKWALK_MAX_NAMELEN );
	MUST( pSym != NULL );
	memset( pSym, 0, sizeof(IMAGEHLP_SYMBOL64) + STACKWALK_MAX_NAMELEN );
	pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
	pSym->MaxNameLength = STACKWALK_MAX_NAMELEN;

	IMAGEHLP_LINE64 Line;
	memset( &Line, 0, sizeof(Line) );
	Line.SizeOfStruct = sizeof(Line);

	HANDLE hThread = GetCurrentThread();
	int curRecursionCount = 0;
	CallstackEntry csEntry;

	for( int frameNum = 0; ; ++frameNum )
	{
		// get next stack frame (StackWalk64(), SymFunctionTableAccess64(), SymGetModuleBase64())
		// if this returns ERROR_INVALID_ADDRESS (487) or ERROR_NOACCESS (998), you can
		// assume that either you are done, or that the stack is so hosed that the next
		// deeper frame could not be found.
		if ( ! pStackWalk64( imageType, hProcess, hThread, &s, &c, myReadProcMem, pSFTA, pSGMB, NULL) )
		{
			break;
		}

		csEntry.offset = s.AddrPC.Offset;
		csEntry.name[0] = 0;
		csEntry.undName[0] = 0;
		csEntry.undFullName[0] = 0;
		csEntry.lineFileName[0] = 0;
		csEntry.lineNumber = 0;

		if( s.AddrPC.Offset == s.AddrReturn.Offset )
		{
			if( curRecursionCount > 1000 )
			{	// Endless Stack
				break;
			}
			++curRecursionCount;
		}
		else
		{
			curRecursionCount = 0;
		}

		if( s.AddrPC.Offset != 0 )
		{
			// procedure info - SymGetSymFromAddr64()
			if( pSGSFA( hProcess, s.AddrPC.Offset, NULL, pSym) != FALSE )
			{
				strcpy_s(csEntry.name, pSym->Name);
				// UnDecorateSymbolName()
				pUDSN( pSym->Name, csEntry.undName, STACKWALK_MAX_NAMELEN, UNDNAME_NAME_ONLY );
				pUDSN( pSym->Name, csEntry.undFullName, STACKWALK_MAX_NAMELEN, UNDNAME_COMPLETE );
			}

			// line number info, NT5.0-method - SymGetLineFromAddr64()
			if ( pSGLFA != NULL )
			{
				DWORD offsetFromLine;
				if( pSGLFA( hProcess, s.AddrPC.Offset, &offsetFromLine, &Line ) != FALSE )
				{
					csEntry.lineNumber = Line.LineNumber;
					strcpy_s( csEntry.lineFileName, Line.FileName );
				}
			}
		}

		OnCallstackEntry( csEntry );

		if (s.AddrReturn.Offset == 0)
		{
			SetLastError( ERROR_SUCCESS );
			break;
		}
	}

	free( pSym );
}

// ---------------------------------------------------------------------------------------------------------------------
void StackWalker::OnCallstackEntry( CallstackEntry& entry )
{
	if( entry.offset == 0 )
		return;

	if( entry.undFullName[0] != 0 )
	{
		strcpy_s( entry.name, entry.undFullName );
	}
	else if( entry.undName[0] != 0 )
	{
		strcpy_s( entry.name, entry.undName );
	}
	else if( entry.name[0] == 0 )
	{
		strcpy_s( entry.name, "(function-name not available)" );
	}
	Denom::ExStackInfo si( entry.lineFileName, entry.lineNumber, entry.name );
	m_output_list->push_back( si );
}

#endif // _DEBUG

// ---------------------------------------------------------------------------------------------------------------------
// Cut off upper part of stack
void CutStackAboveFunc( list<ExStackInfo>& call_stack, const string& func_name )
{
	if( func_name.empty() )
		return;

	list<ExStackInfo>::iterator it = call_stack.begin();
	for( ; it != call_stack.end(); ++it )
	{
		if( func_name == it->func )
		{
			// Cut off upper part of stack including function func_name
			call_stack.erase( call_stack.begin(), ++it );
			break;
		}
	}
}

} // unnamed namespace

#endif // _WIN32

// ---------------------------------------------------------------------------------------------------------------------

namespace Denom {

// ---------------------------------------------------------------------------------------------------------------------
std::list<ExStackInfo>& ReduceCallStack( std::list<ExStackInfo>& call_stack, const std::string& func_name, bool cut_func )
{
	list<ExStackInfo>::iterator it = call_stack.begin();
	for(; it != call_stack.end(); ++it )
	{
		if( it->func == func_name )
		{
			if( !cut_func )
			{
				++it;
			}
			call_stack.erase(it, call_stack.end() );
			break;
		}
	}
	return call_stack;
}

// -----------------------------------------------------------------------------
std::wstring CallStackToString( const std::list<ExStackInfo>& call_stack )
{
	string s;
	s.reserve(1024);
	
	//char buf[20];
	list<ExStackInfo>::const_iterator it = call_stack.begin();
	for( ; it != call_stack.end(); ++it )
	{
		s.append( it->file );

		// TODO: cross
	//	_ultoa_s( it->line, buf, 20, 10 );
		//s.append( " (" );
		//s.append( buf );
		//s.append( "): " );
		s.append( it->func );
		s.append( "\n" );
	}
	return s2w(s);
}

// -----------------------------------------------------------------------------
std::wstring FormatExceptionMessage( const Exception& ex )
{
	wstring s;
	s.reserve( 1024 );
	s.append( L"Error: " );
	s.append( ex.message );
	s.append( L"\n" );

	//wchar_t buf[20];

	if( ex.code != 0 )
	{
		// TODO: cross
		//_ultow_s( ex.code, buf, ARRAY_SIZE(buf), 10 );
		s.append( L"Code: " );
		//s.append( buf );
		s.append( L" (0x" );
		// TODO: cross
		//_ultow_s( ex.code, buf, ARRAY_SIZE(buf), 16 );
		//s.append( buf );
		s.append( L")\n" );
	}


	if( ex.source_info.line != 0 )
	{
		s.append( L"\nPlace:\n" );
		wstring file_str( s2w(ex.source_info.file) );
		size_t left = file_str.rfind( L"..\\" );
		if( left != wstring::npos )
		{
			left += 3;
			s.append( file_str.substr( left ) );
		} else
		{
			s.append( file_str );
		}

		// TODO: cross
		//_ultow_s( ex.source_info.line, buf, 20, 10 );
		//s.append( L" (" );
		//s.append( buf );
		//s.append( L"): " );

		s.append( s2w(ex.source_info.func) );
	}

	if( ex.call_stack.size() != 0)
	{
		s.append( L"\nCall stack:\n" );
		s.append( CallStackToString(ex.call_stack) );
	}
	return s;
}

// =============================================================================
// Exception
// =============================================================================
// -----------------------------------------------------------------------------
Exception::Exception( const std::wstring& message, uint32_t code, const SourceInfo& source_info )
	: message( message ), code(code), source_info(source_info)
{
#if defined(_DEBUG) && defined(_WIN32)
	StackWalker().WriteStack( &call_stack );
	CutStackAboveFunc( call_stack, __FUNCTION__ );
#endif
}

// -----------------------------------------------------------------------------
Exception::Exception( const std::string& message, uint32_t code, const SourceInfo& source_info )
	: message( s2w(message) ), code(code), source_info(source_info)
{
#if defined(_DEBUG) && defined(_WIN32)
	StackWalker().WriteStack( &call_stack );
	CutStackAboveFunc( call_stack, __FUNCTION__ );
#endif
}

// -----------------------------------------------------------------------------
Exception& Exception::SetSourceInfo( const SourceInfo& source_info ) throw()
{
	this->source_info = source_info;
	return *this;
}

} // namespace Denom
