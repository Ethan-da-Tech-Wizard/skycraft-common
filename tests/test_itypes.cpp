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

// Time::SetToNow must read the real clock. The clock is sampled before and
// after the call and the result must match one of the two samples (they can
// differ if a second boundary passes between them).
static void TestTimeSetToNow(void)
{
	time_t		before = std::time(NULL);
	Time		t;
	t.SetToNow();
	time_t		after = std::time(NULL);

	CHECK(t.IsSet(), "SetToNow marks the Time as set");

	struct tm	b, a;
	localtime_r(&before, &b);
	localtime_r(&after, &a);

	bool	matchesBefore =	t.GetHours() == b.tm_hour && t.GetMinutes() == b.tm_min && t.GetSeconds() == b.tm_sec;
	bool	matchesAfter =	t.GetHours() == a.tm_hour && t.GetMinutes() == a.tm_min && t.GetSeconds() == a.tm_sec;

	CHECK(matchesBefore || matchesAfter, "SetToNow stores the current local time (not the old 1:02:03 stub)");
}

// Reverse() is documented by its name as negation: (x, y) -> (-x, -y).
// The old implementation rotated by 90 degrees ((x, y) -> (-y, x)).
static void TestVector2Reverse(void)
{
	Vector2	v(3.0f, 4.0f);
	v.Reverse();

	CHECK(FloatEqual(v.x, -3.0f), "Vector2::Reverse negates x");
	CHECK(FloatEqual(v.y, -4.0f), "Vector2::Reverse negates y");

	// Reversing twice restores the original vector.
	v.Reverse();
	CHECK(FloatEqual(v.x, 3.0f) && FloatEqual(v.y, 4.0f), "Vector2::Reverse twice is the identity");

	// The magnitude is preserved by negation (a rotation would also keep it, so
	// the two component checks above are what distinguish the cases).
	Vector2	w(0.0f, 5.0f);
	w.Reverse();
	CHECK(FloatEqual(w.x, 0.0f) && FloatEqual(w.y, -5.0f), "Vector2::Reverse of (0,5) is (0,-5)");
}

// Dispose() may be called explicitly and then again by the destructor (or by
// a second explicit call). Before the fix the pointer was not reset, so the
// second delete[] freed the same block twice (heap corruption / abort).
static void TestBitstringDoubleDispose(void)
{
	Bitstring	bits(16);
	bits.Clear();
	bits.Set(9);
	CHECK(bits.IsSet(9) && bits.IsClear(8), "Bitstring set/test sanity");

	bits.Dispose();
	bits.Dispose();		// must be a no-op, not a double free

	// Re-allocating after Dispose must work and the destructor then frees once.
	bits.Alloc(8);
	bits.Clear();
	CHECK(bits.IsClear(7), "Bitstring usable again after Dispose + Alloc");
}

int main(void)
{
	TestTimeSetToNow();
	TestVector2Reverse();
	TestBitstringDoubleDispose();

	if(failures)
	{
		std::fprintf(stderr, "%d check(s) failed\n", failures);
		return 1;
	}

	std::printf("ITypes tests passed\n");
	return 0;
}
