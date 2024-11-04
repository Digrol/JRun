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
///
/// Tool for running java applications from sources.

#include <string>
#include <vector>
#include <locale>
#include "log.h"
#include "utils.h"
#include "exception.h"

using std::vector;
using std::string;
using std::wstring;
using namespace Denom;

// ---------------------------------------------------------------------------------------------------------------------
static void OnInvalidParameterInCRT( const wchar_t* expression, const wchar_t* function,
	const wchar_t* file, unsigned int line, uintptr_t pReserved )
{
//	Console::println( L"Invalid parameter detected in function %s.\nFile: %s Line: %d\n", function, file, line );
//	Console::println( L"Expression: %s\n", expression );
	exit(1);
}

// ---------------------------------------------------------------------------------------------------------------------
void Main( const vector<wstring>& args )
{
	for( int i = 0; i < args.size(); ++i )
		Console::println( COLOR_GREEN, L"Аргумент %d: %ls", i, args[ i ].c_str() );

	for( uint32_t c = 0; c < 16; ++c )
		Console::println( c, L"ТЕКСТ текст" );

	for( uint32_t c = 0; c < 16; ++c )
		Console::println( c << 4, L"ТЕКСТ текст" );
}

// ---------------------------------------------------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
	try
	{
		vector< wstring > params = convertCommandLine( argc, argv );
		Main( params );
	}
 	catch( Denom::Exception& ex )
	{
// 		ReduceCallStack( ex.call_stack, __FUNCTION__ );
// 		Console::println( FormatExceptionMessage( ex ).c_str() );
		return (ex.code != 0) ? ex.code : 1;
	}
	catch( const std::exception& ex )
	{
		Console::println( L"Error: %ls", s2w( ex.what() ).c_str() );
		return 1;
	}
	catch ( ... )
	{
		Console::println( L"Unknown Error" );
		return 1;
	}

	return 0;
}
