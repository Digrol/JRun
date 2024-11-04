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
/// Some utilities.

#include "stdinc.h"

#include <chrono>
#include <thread>
#include <locale>
#include <codecvt>

#include "utils.h"
#include "exception.h"

#ifdef _WIN32
#include <windows.h>

// ---------------------------------------------------------------------------------------------------------------------
/// Converts string in 'current' Code Page to wstring
static std::wstring strToWstring( const char* str )
{
	int size = MultiByteToWideChar( CP_ACP, 0, str, -1, nullptr, 0 );
	if( size <= 0 )
		return L"";
	std::wstring sw( size - 1, 0 );
	MultiByteToWideChar( CP_ACP, 0, str, -1, &sw[ 0 ], size );
	return sw;
}

#endif

namespace Denom
{

using std::vector;
using std::string;
using std::wstring;

// ---------------------------------------------------------------------------------------------------------------------
vector< wstring > convertCommandLine( int argc, char* argv[] )
{
	vector< wstring > params;

	for( int i = 0; i < argc; ++i )
	{
		#ifdef _WIN32
			// On windows: args in current Code Page
			params.push_back( strToWstring( argv[ i ] ) );
		#else
			// On X: args in UTF-8
			params.push_back( s2w( argv[ i ] ) );
		#endif // _WIN32
	}
	return params;
}


// ---------------------------------------------------------------------------------------------------------------------
void sleep( uint32_t milliSec )
{
	std::this_thread::sleep_for( std::chrono::milliseconds( milliSec ) );
}

// ---------------------------------------------------------------------------------------------------------------------
uint32_t RangedRand( uint32_t rangeMin, uint32_t rangeMax )
{
	MUST_M( rangeMin < rangeMax, L"Wrong random range" );
	uint16_t hi = rand() + rand() + rand();
	uint16_t lo = rand() + rand() + rand();
	uint64_t num = ((uint32_t)hi << 16) | lo;
	return (uint32_t)(int64_t( num * ((int64_t)rangeMax - (int64_t)rangeMin + 1) / (0xFFFFFFFFLL + 1) ) + (int64_t)rangeMin);
}

// =====================================================================================================================
// Ticker {
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
static int64_t getTicks()
{
	return 0;
// 	LARGE_INTEGER ticks;
// 	QueryPerformanceCounter( &ticks );
// 	return ticks.QuadPart;
}

// ---------------------------------------------------------------------------------------------------------------------
static int64_t getFrequency()
{
	return 0;
	// 	LARGE_INTEGER i;
// 	QueryPerformanceFrequency( &i );
// 	return i.QuadPart;
}

// ---------------------------------------------------------------------------------------------------------------------
Ticker::Ticker()
{
	frequency = getFrequency();
	startTicks = getTicks();
}

// ---------------------------------------------------------------------------------------------------------------------
void Ticker::restart()
{
	startTicks = getTicks();
}

// ---------------------------------------------------------------------------------------------------------------------
int64_t Ticker::diff()
{
	return getTicks() - startTicks;
}

// ---------------------------------------------------------------------------------------------------------------------
int64_t Ticker::diffMs()
{
	return diff() * 1000 / frequency;
}
// =====================================================================================================================
// } Ticker
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
wstring s2w( const string& str )
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.from_bytes( str );
}

// ---------------------------------------------------------------------------------------------------------------------
string w2s( const wstring& wstr )
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.to_bytes( wstr );
}

} // namespace Denom
