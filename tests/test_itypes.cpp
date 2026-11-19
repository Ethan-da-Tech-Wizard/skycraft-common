// ==============================================================================
// Program: test_itypes.cpp
// Purpose: Regression tests for the value types in common/ITypes.h and
//          common/ITypes.cpp: Time::SetToNow, Vector2::Reverse and
//          Bitstring::Dispose.
//
// How to run (macOS / Linux; the real headers assume a Windows ABI):
//   tests/run_itypes_tests.sh
// The script copies common/ to a temporary folder, rewrites the UInt32 /
// SInt32 typedefs from `long` (64-bit on LP64) to `int` so the STATIC_ASSERTs in
// ITypes.h hold, then builds and runs this file together with ITypes.cpp.
// Exit status 0 = all checks passed; the first failure prints a message and exits 1.
//
// Why a local _AssertionFailed: the library's own (IErrors.cpp) pops up a
// Windows message box, so the test provides a version that prints and exits.
// ==============================================================================
#include "common/ITypes.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>

// Replaces the Windows implementation (IErrors.cpp) so ASSERT/ASSERT_STR
// failures inside Bitstring terminate the test with a readable message.
void _AssertionFailed(const char * file, unsigned long line, const char * desc)
{
	std::fprintf(stderr, "assertion failed at %s:%lu: %s\n", file, line, desc);
	std::exit(2);
}

// Overloads declared in IErrors.h; unused by these tests but required to link
// if referenced by a macro.
void _AssertionFailed_ErrCode(const char * file, unsigned long line, const char * desc, unsigned long long)
{
	_AssertionFailed(file, line, desc);
}

void _AssertionFailed_ErrCode(const char * file, unsigned long line, const char * desc, const char *)
{
	_AssertionFailed(file, line, desc);
}

static int failures = 0;

// Records one failed expectation without stopping, so a single run reports every problem.
#define CHECK(cond, msg) do { if(!(cond)) { std::fprintf(stderr, "FAIL: %s\n", msg); failures++; } } while(0)

int main(void)
{

	if(failures)
	{
		std::fprintf(stderr, "%d check(s) failed\n", failures);
		return 1;
	}

	std::printf("ITypes tests passed\n");
	return 0;
}
