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

#ifndef UTILS_H_CBBDDBEE818FD0CE
#define UTILS_H_CBBDDBEE818FD0CE

#include <stdint.h>

typedef const wchar_t* const ConstWChars;
typedef const char* const ConstChars;

#define AS_STR(a) #a
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

namespace Denom
{

// ---------------------------------------------------------------------------------------------------------------------
std::vector< std::wstring > convertCommandLine( int argc, char* argv[] );

// ---------------------------------------------------------------------------------------------------------------------
/// Sleep thread for 'milliSec' milliseconds.
void sleep( uint32_t milliSec );

// ---------------------------------------------------------------------------------------------------------------------
/// Generate pseudo random number
uint32_t rangedRand( uint32_t rangeMin, uint32_t rangeMax );

// ---------------------------------------------------------------------------------------------------------------------
/// Convert string coded in UTF-8 to wstring (UTF-16)
std::wstring s2w( const std::string& str );

// ---------------------------------------------------------------------------------------------------------------------
/// Convert wstring to string in UTF-8
std::string w2s( const std::wstring& wstr );


// =====================================================================================================================
// Ticker
// =====================================================================================================================
/// Class for speed measuring.
/// Ticker t;
/// DoSomeActions();
/// cout << t.GetDiffMs();
class Ticker
{
public:
	/// Start ticker
	Ticker();

	/// Restart ticker
	void restart();

	/// Returns number of ticks since the ticker was started/restarted.
	int64_t diff();

	/// Returns number of milliseconds since the ticker was started/restarted.
	int64_t diffMs();

private:
	int64_t frequency;
	int64_t startTicks;
};

} // namespace Denom

#endif // Header guard