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
/// Exception class.

#ifndef EXCEPTION_H_163D0BE5E68A581C 
#define EXCEPTION_H_163D0BE5E68A581C

#include <stdint.h>
#include <string>
#include <list>

namespace Denom {

// -----------------------------------------------------------------------------
/// Information about one element in call stack.
struct ExStackInfo
{
	std::string file;  // filename
	unsigned line;     // line number
	std::string func;  // function name

	explicit ExStackInfo( const char* file = "", unsigned line = 0, const char* func = "") throw()
		: file(file), line(line), func(func) {}

	// Copy constructor, assignment operator and destructor - default
};

// -----------------------------------------------------------------------------
/// Information about place in source file.
struct SourceInfo
{
	const char* file;  // filename
	unsigned line;     // line number
	const char* func;  // function name

	explicit SourceInfo( const char* const& file = "", unsigned line = 0, const char* const& func = "" ) throw()
		: file(file), line(line), func(func) {}

	// Copy constructor, assignment operator and destructor - default
};

// -----------------------------------------------------------------------------
struct Exception
{
	explicit Exception( const std::wstring& message, uint32_t code = 0, const SourceInfo& source_info = SourceInfo() );
	explicit Exception( const std::string& message, uint32_t code = 0, const SourceInfo& source_info = SourceInfo() );
	Exception& SetSourceInfo( const SourceInfo& source_info ) throw();

	uint32_t code;
	std::wstring message;
	SourceInfo source_info;
	std::list< ExStackInfo > call_stack;
};

// =============================================================================
std::list<ExStackInfo>& ReduceCallStack(std::list<ExStackInfo>& call_stack,
	const std::string& func_name, bool cut_func = false);

// -----------------------------------------------------------------------------
std::wstring FormatExceptionMessage(const Exception& ex);

// =============================================================================

/// Creating object with information about place in source code
#ifdef _DEBUG
#define SOURCE_INFO Denom::SourceInfo(__FILE__, __LINE__, __FUNCTION__)
#else
#define SOURCE_INFO Denom::SourceInfo()
#endif

// -----------------------------------------------------------------------------
/// Throw Denom::Exception with message and source info
#define THROW_M( message ) throw Denom::Exception( message, 0, SOURCE_INFO )
#define THROW_C( code, message ) throw Denom::Exception( message, code, SOURCE_INFO )

/// Add source info and throw ex
#define THROW_EX( ex ) throw ex.SetSourceInfo( SOURCE_INFO )

// -----------------------------------------------------------------------------
/// Test expression and throw Denom::Exception if False.
#define MUST( expression ) { if( !(expression) ) THROW_M( L"" ); }

#define MUST_M( expression, message ) { if( !(expression) ) THROW_M( message ); }

#define MUST_C( expression, code, message ) { if( !(expression) ) THROW_C( code, message ); }

#define MUST_EX( expression, ex ) { if( !(expression) ) THROW_EX( ex ); }

} // namespace Denom

#endif // Header guard
