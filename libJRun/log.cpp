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

#include <cstdarg>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#endif

using std::vector;
using std::string;
using std::wstring;

namespace Denom {

// ---------------------------------------------------------------------------------------------------------------------
wstring formatStr( const wchar_t* format, va_list args )
{
	vector<wchar_t> buf( 512 );

	int len = 0;
	do
	{
		va_list argCopy;
		va_copy( argCopy, args );
		len = vswprintf( &buf[ 0 ], buf.size(), format, argCopy );
		va_end( argCopy );

		if( len < 0 )
		{
			buf.resize( buf.size() * 2 );
		}
		else if( len >= buf.size() )
		{
			buf.resize( (size_t)(len + 1) );
			len = -1;
		}
	}
	while( len < 0 );

	return wstring( &buf[ 0 ], &buf[ 0 ] + len );
}

// ---------------------------------------------------------------------------------------------------------------------
wstring formatStr( const wchar_t* format, ... )
{
	va_list args;
	va_start( args, format );
	wstring result = formatStr( format, args );
	va_end( args );
	return result;
}

/// ====================================================================================================================

namespace Console {

	static void printImpl( const char* utf8StrPtr, unsigned int strSize )
	{
		#ifdef _WIN32
			SetConsoleOutputCP( CP_UTF8 );
			_write( _fileno( stdout ), utf8StrPtr, strSize );
		#else
			write( STDOUT_FILENO, utf8StrPtr, strSize );
		#endif
	}

	// ---------------------------------------------------------------------------------------------------------------------
	static void printImpl( uint32_t color, const char* utf8StrPtr, unsigned int strSize )
	{
		static string colorReset = "\033[0m";
		static string strTextColors[ 16 ] = {
			"\033[30m", // BLACK       [  0 ]
			"\033[34m", // BLUE        [  1 ]
			"\033[32m", // GREEN       [  2 ]
			"\033[36m", // CYAN        [  3 ]
			"\033[31m", // RED         [  4 ]
			"\033[35m", // MAGENTA     [  5 ]
			"\033[33m", // YELLOW      [  6 ]
			"\033[37m", // GRAY        [  7 ]
			"\033[90m", // BLACK_I     [  8 ]
			"\033[94m", // BLUE_I      [  9 ]
			"\033[92m", // GREEN_I     [ 10 ]
			"\033[96m", // CYAN_I      [ 11 ]
			"\033[91m", // RED_I       [ 12 ]
			"\033[95m", // MAGENTA_I   [ 13 ]
			"\033[93m", // YELLOW_I    [ 14 ]
			"\033[97m"  // GRAY_I      [ 15 ]
		};
		static string strBgColors[ 16 ] = {
			"\033[40m", // BLACK       [  0 ]
			"\033[44m", // BLUE        [  1 ]
			"\033[42m", // GREEN       [  2 ]
			"\033[46m", // CYAN        [  3 ]
			"\033[41m", // RED         [  4 ]
			"\033[45m", // MAGENTA     [  5 ]
			"\033[43m", // YELLOW      [  6 ]
			"\033[47m", // GRAY        [  7 ]
			"\033[100m", // BLACK_I     [  8 ]
			"\033[104m", // BLUE_I      [  9 ]
			"\033[102m", // GREEN_I     [ 10 ]
			"\033[106m", // CYAN_I      [ 11 ]
			"\033[101m", // RED_I       [ 12 ]
			"\033[105m", // MAGENTA_I   [ 13 ]
			"\033[103m", // YELLOW_I    [ 14 ]
			"\033[107m"  // GRAY_I      [ 15 ]
		};

		#ifdef _WIN32
			static HANDLE h = ::GetStdHandle( STD_OUTPUT_HANDLE );
			static unsigned short consoleAttr = 0;
			if( consoleAttr == 0 )
			{
				MUST_M( (h != NULL) && (h != INVALID_HANDLE_VALUE), L"Can't open stdout stream" );

				CONSOLE_SCREEN_BUFFER_INFO info;
				GetConsoleScreenBufferInfo( h, &info );
				consoleAttr = info.wAttributes;
			}

			SetConsoleTextAttribute( h, (WORD)color );
			printImpl( utf8StrPtr, strSize );
			SetConsoleTextAttribute( h, consoleAttr );
		#else
			printImpl( strTextColors[ color & 0x0F ].c_str(), strTextColors[ color & 0x0F ].size() );
			int bgIndex = (color >> 4) & 0x0F;
			if( bgIndex != 0 )
				printImpl( strBgColors[ bgIndex ].c_str(), strBgColors[ bgIndex ].size() );
			printImpl( utf8StrPtr, strSize );
			printImpl( colorReset.c_str(), colorReset.size() );
		#endif
	}

	// ---------------------------------------------------------------------------------------------------------------------
	void print( const string& utf8Str )
	{
		printImpl( utf8Str.c_str(), (unsigned int)utf8Str.size() );
	}

	// ---------------------------------------------------------------------------------------------------------------------
	void println( const std::string& utf8Str )
	{
		printImpl( utf8Str.c_str(), (unsigned int)utf8Str.size() );
		printImpl( u8"\n", 1 );
	}

	// ---------------------------------------------------------------------------------------------------------------------
	void print( const std::wstring& text )
	{
		print( w2s(text) );
	}

	// ---------------------------------------------------------------------------------------------------------------------
	void println( const std::wstring& text )
	{
		print( w2s( text ) );
		printImpl( u8"\n", 1 );
	}

	// ---------------------------------------------------------------------------------------------------------------------
	void print( const wchar_t* str, ... )
	{
		va_list args;
		va_start( args, str );
		wstring ws = formatStr( str, args );
		va_end( args );
		print( ws );
	}

	// ---------------------------------------------------------------------------------------------------------------------
	void println( const wchar_t* text, ... )
	{
		va_list args;
		va_start( args, text );
		wstring ws = formatStr( text, args );
		va_end( args );
		print( ws );
		printImpl( u8"\n", 1 );
	}

	// ---------------------------------------------------------------------------------------------------------------------
	void print( uint32_t color, const std::wstring& text )
	{
		string s = w2s( text );
		printImpl( color, s.c_str(), (unsigned int)s.size() );
	}

	// ---------------------------------------------------------------------------------------------------------------------
	void println( uint32_t color, const std::wstring& text )
	{
		string s = w2s( text );
		printImpl( color, s.c_str(), (unsigned int)s.size() );
		printImpl( u8"\n", 1 );
	}

	// ---------------------------------------------------------------------------------------------------------------------
	void print( uint32_t color, const wchar_t* text, ... )
	{
		va_list args;
		va_start( args, text );
		wstring ws = formatStr( text, args );
		va_end( args );
		print( color, ws );
	}

	// ---------------------------------------------------------------------------------------------------------------------
	void println( uint32_t color, const wchar_t* text, ... )
	{
		va_list args;
		va_start( args, text );
		wstring ws = formatStr( text, args );
		va_end( args );
		print( color, ws );
		printImpl( u8"\n", 1 );
	}

} // namespace Console

} // namespace Denom
