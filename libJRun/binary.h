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
/// Class Binary - container of binary data (array of bytes).
/// It is just a transparent shell for std::vector<uint8_t>.
/// Main difference - it works with HEX-format.
/// Also it supports concatenation, comparing and some other utilities.

#pragma once

#include <stdint.h>
#include <vector>
#include <string>

namespace Denom {

// ---------------------------------------------------------------------------------------------------------------------
/// !!! Not allowed to add fields, because destructor in class std::vector is NOT virtual,
/// but we ALLOW CASTING from Binary to vector<uint8_t>
class Binary: public std::vector<uint8_t>
{
private:
	typedef std::vector<uint8_t> _Base;

public:

	// ---------------------------------------------------------------------------------------------------------------------
	// Copy constructor and destructor - from compiler

	// ---------------------------------------------------------------------------------------------------------------------
	/// Create array, containing 'count' bytes 'val' (.
	explicit Binary( size_type count = 0, uint8_t val = 0 ) : _Base( count, val ) {}

	// ---------------------------------------------------------------------------------------------------------------------
	/// Create array from С-string in HEX-format. Examples: "12 9F F0", "53edc01a".
	/// Allowed symbols: 0..9, a..f, A..F, spaces and tabs.
	/// The number of significant characters must be even. For example, "1A 1" - wrong sequence.
	Binary( const char* hex );
	Binary( const wchar_t* hex );

	// ---------------------------------------------------------------------------------------------------------------------
	/// Creating an array from to iterators
	template<typename It>
	Binary( It first, It last ): _Base(first, last) {}

	// ---------------------------------------------------------------------------------------------------------------------
	/// Append array to the end
	Binary& operator+=( const Binary& right );

	// ---------------------------------------------------------------------------------------------------------------------
	/// Append Byte to the end of array
	Binary& operator+=( uint8_t byte );

	// ---------------------------------------------------------------------------------------------------------------------
	/// Convert array to HEX-string in formatted form for printing.
	/// All letters are capitalized.
	/// @param oneSpace - after 'oneSpace' of bytes a space (' ') will be inserted. (fro example 1 - after each byte)
	/// @param twoSpaces - after 'twoSpaces' of bytes 2 spaces("  ") will be inserted.
	/// @param newLine - when insert '\n'.
	/// @param lineShift - shift each line with 'lineShift' spaces.
	std::wstring Hex( uint32_t oneSpace = 1, uint32_t twoSpaces = 0, uint32_t newLine = 0, uint32_t lineShift = 0 ) const;
	/// All letters are lowercase
	std::wstring hex( uint32_t oneSpace = 1, uint32_t twoSpaces = 0, uint32_t newLine = 0, uint32_t lineShift = 0 ) const;

	// ---------------------------------------------------------------------------------------------------------------------
	/// Interpret array as unsigned integer.
	/// First byte is highest (BigEndian).
	/// Allowed size of array is from 0 to 'size' bytes, where 'size' is sizeof(IntegerType).
	/// Exapmle:
	/// Binary b( "01 00 00" );
	/// b.U32(); // = 65536
	uint16_t U16() const;
	uint32_t U32() const;
	uint64_t U64() const;

	// ---------------------------------------------------------------------------------------------------------------------
	/// Make a slice from array, like 'substr' for 'string'.
	/// Checking for array out of bounds.
	/// @param offset - fragment starts at this offset.
	/// @param count - number of bytes in fragment.
	Binary slice( size_type offset, size_type count ) const;

	// ---------------------------------------------------------------------------------------------------------------------
	/// First 'count' bytes of array.
	Binary first( size_type count ) const;

	// ---------------------------------------------------------------------------------------------------------------------
	/// Last 'count' bytes of array.
	Binary last( size_type count ) const;

	// ---------------------------------------------------------------------------------------------------------------------
	/// Fill array with random data.
	/// @param size - desired size of array.
	/// @return - this.
	Binary& random( size_type size );

	// -----------------------------------------------------------------------------------------------------------------
	/// Get bit in byte.
	/// @param index - index of byte in array.
	/// @param bitNum - bit number in byte [0-7]. The least significant bit is 0, the most significant bit is 7.
	/// @return true if bit set.
	bool getBit( size_type index, uint8_t bitNum ) const;

	/// Set bit in byte (set to 1).
	void setBit( size_type index, uint8_t bitNum );

	/// Reset bit in byte (set to 0).
	void resetBit( size_type index, uint8_t bitNum );

	/// Set bit to 'bitValue' (0 or not 0).
	void writeBit( size_type index, uint8_t bitNum, bool bitValue );

	// -----------------------------------------------------------------------------------------------------------------
	/// Load file as byte array.
	/// @return - this.
	Binary& loadFromFile( const std::wstring& filename );

	/// Save this array to file.
	void saveToFile( const std::wstring& filename );

	// -----------------------------------------------------------------------------------------------------------------
	/// Increment by 1. Interpret array as BigEndian unsigned number.
	/// Example:  Binary("00 02 FF FF").increment()  ->  "00 03 00 00"
	void increment();

	/// Example:  Binary("00 33 FF 00").decrement()  ->  "00 33 FE FF"
	void decrement();

private:
	friend bool operator==( const Binary& left, const Binary& right );
};

// -----------------------------------------------------------------------------------------------------------------
/// Concatenate 2 Binary arrays
Binary operator+( const Binary& left, const Binary& right );

/// Append a byte to Binary array
Binary operator+( const Binary& bin, uint8_t byte );
Binary operator+( uint8_t byte, const Binary& bin );

/// Comparing 2 binary arrays for equality
bool operator==( const Binary& left, const Binary& right );

/// Inequality comparison of 2 binary arrays
bool operator!=( const Binary& left, const Binary& right );

/// XOR two binary arrays (should be same size)
Binary operator^( const Binary& left, const Binary& right );

/// OR two binary arrays (should be same size)
Binary operator|( const Binary& left, const Binary& right );

/// AND two binary arrays (should be same size)
Binary operator&( const Binary& left, const Binary& right );



} // namespace Denom
