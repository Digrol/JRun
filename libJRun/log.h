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

#pragma once

#include <string>
#include <memory>

namespace Denom {

std::wstring formatStr( const wchar_t* format, ... );
std::wstring formatStr( const wchar_t* format, va_list a );

// ---------------------------------------------------------------------------------------------------------------------
/// Text Colors
const uint32_t COLOR_INTENS    = 0x08;
const uint32_t COLOR_BG_INTENS = 0x80;

const uint32_t COLOR_BLACK          = 0x00;
const uint32_t COLOR_BLUE           = 0x01;
const uint32_t COLOR_GREEN          = 0x02;
const uint32_t COLOR_CYAN           = COLOR_GREEN | COLOR_BLUE;
const uint32_t COLOR_RED            = 0x04;
const uint32_t COLOR_MAGENTA        = COLOR_BLUE  | COLOR_RED;
const uint32_t COLOR_YELLOW         = COLOR_GREEN | COLOR_RED;
const uint32_t COLOR_GRAY           = COLOR_GREEN | COLOR_BLUE | COLOR_RED;
const uint32_t COLOR_DARK_GRAY      = COLOR_BLACK   | COLOR_INTENS;
const uint32_t COLOR_BLUE_I         = COLOR_BLUE    | COLOR_INTENS;
const uint32_t COLOR_GREEN_I        = COLOR_GREEN   | COLOR_INTENS;
const uint32_t COLOR_CYAN_I         = COLOR_CYAN    | COLOR_INTENS;
const uint32_t COLOR_RED_I          = COLOR_RED     | COLOR_INTENS;
const uint32_t COLOR_MAGENTA_I      = COLOR_MAGENTA | COLOR_INTENS;
const uint32_t COLOR_YELLOW_I       = COLOR_YELLOW  | COLOR_INTENS;
const uint32_t COLOR_WHITE          = COLOR_GRAY    | COLOR_INTENS;

const uint32_t COLOR_BG_BLACK       = 0x00;
const uint32_t COLOR_BG_BLUE        = 0x10;
const uint32_t COLOR_BG_GREEN       = 0x20;
const uint32_t COLOR_BG_CYAN        = COLOR_BG_GREEN | COLOR_BG_BLUE;
const uint32_t COLOR_BG_RED         = 0x40;
const uint32_t COLOR_BG_MAGENTA     = COLOR_BG_BLUE  | COLOR_BG_RED;
const uint32_t COLOR_BG_YELLOW      = COLOR_BG_GREEN | COLOR_BG_RED;
const uint32_t COLOR_BG_GRAY        = COLOR_BG_GREEN | COLOR_BG_BLUE | COLOR_BG_RED;
const uint32_t COLOR_BG_DARK_GRAY_I = COLOR_BG_BLACK   | COLOR_BG_INTENS;
const uint32_t COLOR_BG_BLUE_I      = COLOR_BG_BLUE    | COLOR_BG_INTENS;
const uint32_t COLOR_BG_GREEN_I     = COLOR_BG_GREEN   | COLOR_BG_INTENS;
const uint32_t COLOR_BG_CYAN_I      = COLOR_BG_CYAN    | COLOR_BG_INTENS;
const uint32_t COLOR_BG_RED_I       = COLOR_BG_RED     | COLOR_BG_INTENS;
const uint32_t COLOR_BG_MAGENTA_I   = COLOR_BG_MAGENTA | COLOR_BG_INTENS;
const uint32_t COLOR_BG_YELLOW_I    = COLOR_BG_YELLOW  | COLOR_BG_INTENS;
const uint32_t COLOR_BG_WHITE       = COLOR_BG_GRAY    | COLOR_BG_INTENS;

const uint32_t COLOR_ERROR = COLOR_RED_I;

// ---------------------------------------------------------------------------------------------------------------------
namespace Console
{
	void print( const std::string& utf8Str );
	void println( const std::string& utf8Str );

	void print( const std::wstring& str );
	void println( const std::wstring& text );

	void print( const wchar_t* str, ... );
	void println( const wchar_t* text, ... );

	/// @param color - see constants COLOR_*. Symbol colors and BackGround colors can be combined.
	/// Example: COLOR_BG_WHITE | COLOR_RED
	void print( uint32_t color, const std::wstring& text );
	void println( uint32_t color, const std::wstring& text );

	void print( uint32_t color, const wchar_t* text, ... );
	void println( uint32_t color, const wchar_t* text, ... );
}

} // namespace Denom
