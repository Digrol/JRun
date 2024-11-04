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
/// Class Binary - Array of bytes.

#include "stdinc.h"

#include "exception.h"
#include "binary.h"
#include "utils.h"
// #include <sys/stat.h>
// #include <string.h>
// #include <stdlib.h>
// #include <fstream>
// #include <iostream>
// #include <iterator>

using std::wstring;

namespace {

// ---------------------------------------------------------------------------------------------------------------------
/// Convert lower nibble to char or wchar
char nibbleChars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
char nibblechars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
wchar_t nibbleWChars[ 16 ] = { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F' };
wchar_t nibblewchars[ 16 ] = { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'a', L'b', L'c', L'd', L'e', L'f' };

// ---------------------------------------------------------------------------------------------------------------------
/// Converts HEX character into an appropriate number.
/// @param hexSymbol - '0'..'9', 'A'..'F', 'a'..'f', codes of these characters in ASCII.
/// @return - hexadecimal digit 0x00..0x0F, or 0xFF.
/// Examples:
/// 'D' (0x44)  ->  0x0D.
/// 'e' (0x65)  ->  0x0E.
/// '8' (0x38)  ->  0x08.
/// 'r'  ->  0xFF
uint8_t char2nibble( char hexSymbol )
{
	if( (hexSymbol >= '0') && (hexSymbol <= '9') )
		return hexSymbol - '0';

	if( (hexSymbol >= 'A') && (hexSymbol <= 'F') )
		return hexSymbol - 'A' + 0x0A;

	if( (hexSymbol >= 'a') && (hexSymbol <= 'f') )
		return hexSymbol - 'a' + 0x0A;

	return 0xFF;
}

// ---------------------------------------------------------------------------------------------------------------------
/// @param hexSymbol - L'0'..L'9', L'A'..L'F', L'a'..L'f', codes of these characters.
/// @return - hexadecimal digit 0x00..0x0F, or 0xFF.
uint8_t wchar2nibble( wchar_t ch )
{
	if( (ch >= L'0') && (ch <= L'9') )
		return ch - L'0';

	if( (ch >= L'A') && (ch <= L'F') )
		return ch - L'A' + 0x0A;

	if( (ch >= L'a') && (ch <= L'f') )
		return ch - L'a' + 0x0A;

	return 0xFF;
}

} // namespace


namespace Denom {

// ---------------------------------------------------------------------------------------------------------------------
Binary::Binary( const char* hex )
{
	MUST_M( hex != NULL, L"NULL Ptr" );

	reserve( strlen(hex) >> 1 );
	uint8_t nibble = 0;     // current nibble
	uint8_t highNibble = 0;
	bool flag = false;      // Set if got 1 nibble

	for( ; *hex; ++hex )
	{
		nibble = char2nibble( *hex );
		if( nibble != 0xFF )
		{
			if ( flag )
			{
				push_back( highNibble | nibble);
				flag = false; // got byte
			}
			else
			{
				highNibble = nibble << 4;
				flag = true;
			}
		}
		else
		{
			MUST_M( isspace( *hex ) != 0, L"Wrong symbol in HEX-string" );
		}
	}
	MUST_M( !flag, L"Odd number of hex numbers in HEX-string" );
}

// ---------------------------------------------------------------------------------------------------------------------
Binary::Binary( const wchar_t* hex )
{
	MUST_M( hex != NULL, L"NULL Ptr" );

	reserve( wcslen(hex) >> 1 );
	uint8_t nibble = 0;       // current nibble
	uint8_t high_nibble = 0;
	bool flag = false;        // Set if got 1 nibble

	for( ; *hex; ++hex )
	{
		nibble = wchar2nibble( *hex );
		if( nibble != 0xFF )
		{
			if( flag )
			{
				push_back( high_nibble | nibble );
				flag = false; // got byte
			}
			else
			{
				high_nibble = nibble << 4;
				flag = true;
			}
		}
		else
		{
			MUST_M( iswspace(*hex) != 0, L"Wrong symbol in HEX-string" );
		}
	}
	MUST_M( !flag, L"Odd number of hex numbers in HEX-string" );
}

// ---------------------------------------------------------------------------------------------------------------------
Binary& Binary::operator+=( const Binary& right )
{
	reserve( size() + right.size() );
	copy( right.begin(), right.end(), back_inserter(*this) );
	return *this;
}

// ---------------------------------------------------------------------------------------------------------------------
Binary& Binary::operator+=( uint8_t byte )
{
	push_back( byte );
	return *this;
}

// ---------------------------------------------------------------------------------------------------------------------
wstring Binary::Hex( uint32_t oneSpace, uint32_t twoSpaces, uint32_t newLine, uint32_t lineShift ) const
{
	if( empty() )
	{
		return wstring();
	}

	wstring res;
	res.reserve( size() * 3 );

	wstring shiftStr( lineShift, L' ' );
	res += shiftStr;
	uint8_t byte = at( 0 );
	res += nibbleWChars[ byte >> 4 ];
	res += nibbleWChars[ byte & 0x0F ];

	for( size_type i = 1; i < size(); ++i )
	{
		if( newLine && !(i % newLine) )
		{
			res += L'\n';
			res += shiftStr;
		}
		else if( twoSpaces && !(i % twoSpaces) )
		{
			res += L"  ";
		}
		else if( oneSpace && !(i % oneSpace) )
		{
			res += L" ";
		}
		byte = at( i );
		res += nibbleWChars[ byte >> 4 ];
		res += nibbleWChars[ byte & 0x0F ];
	}
	return res;
}


// ---------------------------------------------------------------------------------------------------------------------
wstring Binary::hex( uint32_t oneSpace, uint32_t twoSpaces, uint32_t newLine, uint32_t lineShift ) const
{
	if( empty() )
	{
		return wstring();
	}

	wstring res;
	res.reserve( size() * 3 );

	wstring shiftStr( lineShift, L' ' );
	res += shiftStr;
	uint8_t byte = at( 0 );
	res += nibblewchars[ byte >> 4 ];
	res += nibblewchars[ byte & 0x0F ];

	for( size_type i = 1; i < size(); ++i )
	{
		if( newLine && !(i % newLine) )
		{
			res += L'\n';
			res += shiftStr;
		}
		else if( twoSpaces && !(i % twoSpaces) )
		{
			res += L"  ";
		}
		else if( oneSpace && !(i % oneSpace) )
		{
			res += L" ";
		}
		byte = at( i );
		res += nibblewchars[ byte >> 4 ];
		res += nibblewchars[ byte & 0x0F ];
	}
	return res;
}

/*
// ---------------------------------------------------------------------------------------------------------------------
Binary& Binary::loadFromFile( const wstring& filename )
{
	clear();

	struct stat statFile;
	MUST_M( stat( w2s( filename ).c_str(), &statFile ) == 0, L"Can't get file size: " + filename );

	if( statFile.st_size == 0 )
	{
		return *this;
	}

	FILE* f = NULL;
	errno_t err = fopen_s( &f, w2s( filename ).c_str(), "rb" );
	MUST_M( (err == 0) && (f != NULL), L"Can't open file: " + filename );

	resize( statFile.st_size );
	uint8_t* p = &(*this)[ 0 ];

	uint8_t buf[ 60000 ];
	size_t bytes_read = fread( buf, 1, 60000, f );
	size_t sz = bytes_read;
	while( bytes_read != 0 )
	{
		MUST( sz <= size() );
		std::copy( buf, buf + bytes_read, p );
		p += bytes_read;
		bytes_read = fread( buf, 1, 60000, f );
		sz += bytes_read;
	}
	fclose( f );

	return *this;
}

// ---------------------------------------------------------------------------------------------------------------------
void Binary::saveToFile( const std::wstring& filename )
{
	FILE* f = NULL;
	errno_t err = fopen_s( &f, w2s( filename ).c_str(), "wb" );
	MUST_M( (err == 0) && (f != NULL), L"Can't open file: " + filename );

	size_t bytes_write = 0;
	if( !empty() )
	{
		bytes_write = fwrite( &(*this)[ 0 ], 1, size(), f );
	}
	fflush( f );
	fclose( f );

	MUST_M( bytes_write == size(), L"Can't write all data from Binary to file" );
}

// ---------------------------------------------------------------------------------------------------------------------
uint16_t Binary::U16() const
{
	switch( size() )
	{
	case 0:
		return 0;
	case 1:
		return (*this)[0];
	case 2:
		return ((uint16_t)((*this)[0]) << 8) | (*this)[1];
	default:
		THROW_M( L"Size of Binary can't be more than 2 bytes to interpret it as U16" );
	}
	return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
uint32_t Binary::U32() const
{
	if( empty() )
	{
		return 0;
	}

	uint32_t num = 0;
	size_t len = size();

	MUST_M( sizeof(num) >= size(), L"Size of Binary can't be more than 4 bytes to interpret it as U32" );

	uint8_t* p = (uint8_t*)&num;
	do 
	{
		--len;
		*p = (*this)[ len ];
		++p;
	}
	while( len != 0 );

	return num;
}

// -----------------------------------------------------------------------------
uint64_t Binary::U64() const
{
	if( empty() )
	{
		return 0;
	}

	uint64_t num = 0;
	size_t len = size();

	MUST_M( sizeof(num) >= len, L"Size of Binary can't be more than 8 bytes to interpret it as U64" );

	uint8_t* p = (uint8_t*)&num;
	do 
	{
		--len;
		*p = (*this)[ len ];
		++p;
	}
	while( len != 0 );

	return num;
}

// -----------------------------------------------------------------------------
Binary& Binary::random( size_type size )
{
	clear();
	reserve( size );
	for( unsigned i = 0; i < size; ++i )
	{
		push_back( (uint8_t)RangedRand( 0, 255 ) );
	}
	return *this;
}

// -----------------------------------------------------------------------------
Binary Binary::slice( size_type offset, size_type count ) const
{
	MUST_M( (offset + count) <= size(), L"Out of 'Binary' borders in 'Slice'" );
	const_iterator it_begin = begin() + offset;
	return Binary( it_begin, it_begin + count );
}

// -----------------------------------------------------------------------------------------------------------------
Binary Binary::first( size_type count ) const
{
	MUST_M( count <= size(), L"Wrong 'count' in Binary.first()" );
	return Binary( begin(), begin() + count );
}

// -----------------------------------------------------------------------------------------------------------------
Binary Binary::last( size_type count ) const
{
	MUST_M( count <= size(), "Wrong 'count' in Binary.last()" );
	return Binary( end() - count, end() );
}

// ---------------------------------------------------------------------------------------------------------------------
bool Binary::getBit( size_type index, uint8_t bitNum ) const
{
	MUST_M( (bitNum >= 0) && (bitNum <= 7), "Wrong 'bitNum'" );
	MUST_M( index < size(), "Binary index out of bounds" );
	return ((*this)[ index ] & (1 << bitNum)) != 0;
}

// ---------------------------------------------------------------------------------------------------------------------
void Binary::setBit( size_type index, uint8_t bitNum )
{
	MUST_M( (bitNum >= 0) && (bitNum <= 7), "Wrong 'bitNum'" );
	MUST_M( index < size(), "Binary index out of bounds" );
	(*this)[ index ] |= (1 << bitNum);
}

// ---------------------------------------------------------------------------------------------------------------------
void Binary::resetBit( size_type index, uint8_t bitNum )
{
	MUST_M( (bitNum >= 0) && (bitNum <= 7), "Wrong 'bitNum'" );
	MUST_M( index < size(), "Binary index out of bounds" );
	(*this)[ index ] &= ~(1 << bitNum);
}

// ---------------------------------------------------------------------------------------------------------------------
void Binary::writeBit( size_type index, uint8_t bitNum, bool bitValue )
{
	if( bitValue )
		setBit( index, bitNum );
	else
		resetBit( index, bitNum );
}

// ---------------------------------------------------------------------------------------------------------------------
void Binary::increment()
{
	if( empty() )
		return;

	size_type i = size();
	do
	{
		--i;
		++(*this)[i];
	}
	while (i && !(*this)[i]);
}

// ---------------------------------------------------------------------------------------------------------------------
void Binary::decrement()
{
	if( empty() )
		return;

	size_type i = size();
	do
	{
		--i;
		--(*this)[i];
	}
	while( i && ((*this)[i] == 0xFF) );
}

// ---------------------------------------------------------------------------------------------------------------------
bool operator==(const Binary& left, const Binary& right)
{
	return operator==(
		static_cast<const Binary::_Base& >(left),
		static_cast<const Binary::_Base& >(right)
	);
}

// ---------------------------------------------------------------------------------------------------------------------
bool operator!=( const Binary& left, const Binary& right )
{
	return !operator==(left, right);
}

// ---------------------------------------------------------------------------------------------------------------------
Binary operator+( const Binary& left, const Binary& right )
{
	return Binary(left) += right;
}

// ---------------------------------------------------------------------------------------------------------------------
Binary operator+( const Binary& binary, uint8_t byte )
{
	Binary newbin(binary);
	newbin.push_back(byte);
	return newbin;
}

// ---------------------------------------------------------------------------------------------------------------------
Binary operator+( uint8_t byte, const Binary& binary )
{
	Binary res;
	res.reserve( 1 + binary.size() );
	res.push_back( byte );
	res += binary;
	return res;
}

// ---------------------------------------------------------------------------------------------------------------------
Binary operator^( const Binary& left, const Binary& right )
{
	size_t sz = left.size();
	MUST_M( sz == right.size(), L"Size of Binary arrays must be equal in operator '^'" );

	Binary result;
	result.reserve(sz);
	for( size_t i = 0; i < sz; ++i )
	{
		result.push_back( left[i] ^ right[i] );
	}
	return result;
}

// ---------------------------------------------------------------------------------------------------------------------
Binary operator|( const Binary& left, const Binary& right )
{
	size_t sz = left.size();
	MUST_M( left.size() == right.size(), L"Size of Binary arrays must be equal in operator '|'" );

	Binary result;
	result.reserve(sz);
	for( unsigned i = 0; i < sz; ++i )
	{
		result.push_back( left[i] | right[i] );
	}
	return result;
}

// ---------------------------------------------------------------------------------------------------------------------
Binary operator&(const Binary& left, const Binary& right)
{
	size_t sz = left.size();
	MUST_M( left.size() == right.size(), L"Size of Binary arrays must be equal in operator '&'" );

	Binary result;
	result.reserve(sz);
	for( unsigned i = 0; i < sz; ++i )
	{
		result.push_back(left[i] & right[i]);
	}
	return result;
}

*/
} // namespace Denom
