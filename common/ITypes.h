// ==============================================================================
// Program: ITypes.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Fixed-width integer typedefs and small value types shared by every
//          SKSE file: byte-swap helpers, FourCC/version/colour packing macros,
//          bit containers, Time, Vector2 and Vector3.
//          Inputs: none (header only, plus ITypes.cpp for Bitstring).
//          Outputs: the types and inline helpers below.
//          Architecture: layouts must match the game's binary structures, so
//          sizes are fixed by typedef and checked with STATIC_ASSERT.
//
// How it fits: included (directly or via IPrefix.h) by nearly every other file
// in this repository and in skycraft-skse64; it includes only IErrors.h for
// ASSERT / STATIC_ASSERT. Nothing here allocates except Bitstring.
//
// Known limits: the typedefs assume a Windows (LLP64) compiler; see NOTE below.
// ==============================================================================

#pragma once

#include "common/IErrors.h"

// MSVC-only pragma that silences warning C4221 (initializing an aggregate with
// the address of an automatic variable); other compilers ignore it.
#pragma warning(disable: 4221)
#include <cmath>
#include <ctime>	// time(), localtime_r / localtime_s for Time::SetToNow

// Sized types. Names follow the SKSE convention: UIntN / SIntN are unsigned /
// signed N-bit integers, Float32 / Float64 are IEEE floats. The comments on
// each line are Doxygen (//!<) so tools can show them.
// NOTE: UInt32/SInt32 are based on `long`, which is 32 bits under Windows
// (MSVC, the only supported target) but 64 bits on LP64 platforms such as Linux
// and macOS, so these headers must be built with a Windows ABI.
typedef unsigned char		UInt8;		//!< An unsigned 8-bit integer value
typedef unsigned short		UInt16;		//!< An unsigned 16-bit integer value
typedef unsigned long		UInt32;		//!< An unsigned 32-bit integer value
typedef unsigned long long	UInt64;		//!< An unsigned 64-bit integer value
typedef signed char			SInt8;		//!< A signed 8-bit integer value
typedef signed short		SInt16;		//!< A signed 16-bit integer value
typedef signed long			SInt32;		//!< A signed 32-bit integer value
typedef signed long long	SInt64;		//!< A signed 64-bit integer value
typedef float				Float32;	//!< A 32-bit floating point value
typedef double				Float64;	//!< A 64-bit floating point value
/**
 * Sign-extends a 16-bit value held in the low bits of a 32-bit word.
 * If bit 15 is set the upper 16 bits are filled with ones.
 * @param in Value whose low 16 bits are the number to extend.
 * @return Sign-extended 32-bit value (e.g. 0x8000 -> 0xFFFF8000).
 */

inline UInt32 Extend16(UInt32 in)
{
	return (in & 0x8000) ? (0xFFFF0000 | in) : in;
}

/** Same as Extend16() for an 8-bit value (0x80 -> 0xFFFFFF80). */
inline UInt32 Extend8(UInt32 in)
{
	return (in & 0x80) ? (0xFFFFFF00 | in) : in;
}

// Byte-order helpers: reverse the bytes of 16/32/64-bit integers. Used when
// reading data written with the opposite endianness (e.g. big-endian console
// files read on a little-endian PC).
inline UInt16 Swap16(UInt16 in)
{
	return	((in >> 8) & 0x00FF) |
			((in << 8) & 0xFF00);
}

inline UInt32 Swap32(UInt32 in)
{
	return	((in >> 24) & 0x000000FF) |
			((in >>  8) & 0x0000FF00) |
			((in <<  8) & 0x00FF0000) |
			((in << 24) & 0xFF000000);
}

/** Swaps each 32-bit half's bytes and exchanges the halves, reversing all 8 bytes. */
inline UInt64 Swap64(UInt64 in)
{
	UInt64	temp;

	temp = Swap32(in);
	temp <<= 32;
	temp |= Swap32(in >> 32);

	return temp;
}

// In-place byte swap of a float/double by reinterpreting its bits as an
// integer. Relies on sizeof(float) == 4 and sizeof(double) == 8; the pointer
// cast violates strict aliasing in theory but works with MSVC.
inline void SwapFloat(float * in)
{
	UInt32	* temp = (UInt32 *)in;

	*temp = Swap32(*temp);
}

inline void SwapDouble(double * in)
{
	UInt64	* temp = (UInt64 *)in;

	*temp = Swap64(*temp);
}

/**
 * Runtime endianness test: stores 0x1234 and checks which byte comes first in memory.
 * @return true on big-endian CPUs (first byte is 0x12), false on x86/x64.
 */
inline bool IsBigEndian(void)
{
	union
	{
		UInt16	u16;
		UInt8	u8[2];
	} temp;

	temp.u16 = 0x1234;
	
	return temp.u8[0] == 0x12;
}

/** Logical inverse of IsBigEndian(). */
inline bool IsLittleEndian(void)
{
	return !IsBigEndian();
}

// Packs four characters into a 32-bit value, first character in the lowest
// byte (so the in-memory byte order spells the string on little-endian CPUs).
// Macro arguments are masked to 8 bits; they are not parenthesised, so pass
// plain character literals or constants.
#define CHAR_CODE(a, b, c, d)	(((a & 0xFF) << 0) | ((b & 0xFF) << 8) | ((c & 0xFF) << 16) | ((d & 0xFF) << 24))
// Compile-time (constant expression) byte swaps, usable in initialisers and
// switch labels; they evaluate their argument several times, so avoid side effects.
#define MACRO_SWAP16(a)			((((a) & 0x00FF) << 8) | (((a) & 0xFF00) >> 8))
#define MACRO_SWAP32(a)			((((a) & 0x000000FF) << 24) | (((a) & 0x0000FF00) << 8) | (((a) & 0x00FF0000) >> 8) | (((a) & 0xFF000000) >> 24))

// Packs a version as 12 bits primary | 12 bits secondary | 8 bits sub (bits
// 31..20, 19..8, 7..0); the following macros extract each field. Example:
// VERSION_CODE(1, 0, 0) == 0x00100000.
#define VERSION_CODE(primary, secondary, sub)	(((primary & 0xFFF) << 20) | ((secondary & 0xFFF) << 8) | ((sub & 0xFF) << 0))
#define VERSION_CODE_PRIMARY(in)				((in >> 20) & 0xFFF)
#define VERSION_CODE_SECONDARY(in)				((in >> 8) & 0xFFF)
#define VERSION_CODE_SUB(in)					((in >> 0) & 0xFF)

// Packs ARGB into one 32-bit value, 8 bits per channel (alpha in the top byte);
// the COLOR_* macros extract a channel.
#define MAKE_COLOR(a, r, g, b)	(((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | ((b & 0xFF) << 0))
#define COLOR_ALPHA(in)			((in >> 24) & 0xFF)
#define COLOR_RED(in)			((in >> 16) & 0xFF)
#define COLOR_GREEN(in)			((in >> 8) & 0xFF)
#define COLOR_BLUE(in)			((in >> 0) & 0xFF)

/**
 *	A 64-bit variable combiner
 *	
 *	Useful for endian-independent value extraction. Each struct member names
 *	the sub-fields from the most significant (a) to the least significant, so
 *	u32.a is always the high word whatever the CPU's byte order.
 *	NOTE: the declaration order of the fields (b before a, d before a ...) is
 *	the little-endian memory order; on a big-endian CPU the names would be reversed.
 */
/**
 *	A 64-bit variable combiner
 *	
 *	Useful for endian-independent value extraction.
 */
union VarCombiner
{
	UInt64	u64;
	SInt64	s64;
	double	f64;
	struct { UInt32 b; UInt32 a; } u32;
	struct { SInt32 b; SInt32 a; } s32;
	struct { float  b; float  a; } f32;
	struct { UInt16 d; UInt16 c; UInt16 b; UInt16 a; } u16;
	struct { SInt16 d; SInt16 c; SInt16 b; SInt16 a; } s16;
	struct { UInt8  h; UInt8  g; UInt8  f; UInt8  e;
			 UInt8  d; UInt8  c; UInt8  b; UInt8  a; } u8;
	struct { SInt8  h; SInt8  g; SInt8  f; SInt8  e;
			 SInt8  d; SInt8  c; SInt8  b; SInt8  a; } s8;
};

/**
 *	A bitfield.
 *
 *	@tparam T Storage type (UInt8, UInt16 or UInt32). Methods take UInt32 masks.
 */
/**
 *	A bitfield.
 */
template <typename T>
// Typed set of flag helpers over an integer `field`. Note that the mask
// parameters are UInt32, so for 64-bit use the upper bits are not reachable.
// ExtractField(shift, length) assumes 1 <= length <= 32.
// Typed set of flag helpers over an integer `field`. Note that the mask
// parameters are UInt32, so for Bitfield64-sized use the upper bits are not
// reachable. ExtractField(shift, length) assumes length <= 32.
class Bitfield
{
	public:
				// The constructor does not initialise `field`; call Clear() or RawSet() first.
				Bitfield()					{ }
				~Bitfield()					{ }
		
		void	Clear(void)					{ field = 0; }						//!< Clears all bits
		void	RawSet(UInt32 data)			{ field = data; }					//!< Modifies all bits
		
		void	Set(UInt32 data)			{ field |= data; }					//!< Sets individual bits
		void	Clear(UInt32 data)			{ field &= ~data; }					//!< Clears individual bits
		void	UnSet(UInt32 data)			{ Clear(data); }					//!< Clears individual bits
		void	Mask(UInt32 data)			{ field &= data; }					//!< Masks individual bits
		void	Toggle(UInt32 data)			{ field ^= data; }					//!< Toggles individual bits
		void	Write(UInt32 data, bool state)
											{ if(state) Set(data); else Clear(data); }
		
		T		Get(void) const				{ return field; }					//!< Gets all bits
		T		Get(UInt32 data) const		{ return field & data; }			//!< Gets individual bits
		T		Extract(UInt32 bit) const	{ return (field >> bit) & 1; }		//!< Extracts a bit
		T		ExtractField(UInt32 shift, UInt32 length)					//!< Extracts a series of bits
											{ return (field >> shift) & (0xFFFFFFFF >> (32 - length)); }
		
		bool	IsSet(UInt32 data) const	{ return ((field & data) == data) ? true : false; }	//!< Are all these bits set?
		bool	IsUnSet(UInt32 data) const	{ return (field & data) ? false : true; }			//!< Are all these bits clear?
		bool	IsClear(UInt32 data) const	{ return IsUnSet(data); }							//!< Are all these bits clear?
	
	private:
		T		field;	//!< bitfield data
};

typedef Bitfield <UInt8>	Bitfield8;		//!< An 8-bit bitfield
typedef Bitfield <UInt16>	Bitfield16;		//!< A 16-bit bitfield
typedef Bitfield <UInt32>	Bitfield32;		//!< A 32-bit bitfield

// Compile-time checks that the wrappers add no storage over the raw integer.
STATIC_ASSERT(sizeof(Bitfield8) == 1);
STATIC_ASSERT(sizeof(Bitfield16) == 2);
STATIC_ASSERT(sizeof(Bitfield32) == 4);

/**
 *	A bitstring
 *	
 *	Essentially a long bitvector: one bit per index, heap-allocated, 8-bit
 *	aligned. Implemented in ITypes.cpp. Out-of-range indexes assert.
 */
/**
 *	A bitstring
 *	
 *	Essentially a long bitvector.
 */
class Bitstring
{
	public:
				Bitstring();
				Bitstring(UInt32 inLength);
				~Bitstring();

		// Alloc(bits) rounds up to a whole byte count; previous storage is freed.
		void	Alloc(UInt32 inLength);
		void	Dispose(void);

		void	Clear(void);
		void	Clear(UInt32 idx);
		void	Set(UInt32 idx);

		bool	IsSet(UInt32 idx);
		bool	IsClear(UInt32 idx);

	// Bytes of storage; bit index idx lives in data[idx >> 3], bit (idx & 7).
	private:
		UInt8	* data;
		UInt32	length;	//!< length in bytes
};

/**
 *	Time storage
 *
 *	A simple seconds/minutes/hours holder (no date) with a "has data" flag.
 *	Note the argument order of Set(): seconds, minutes, hours.
 */
/**
 *	Time storage
 */
// SetToNow() reads the local clock (localtime_s on Windows, localtime_r elsewhere,
// both thread-safe).
class Time
{
	public:
				Time()				{ Clear(); }
				~Time()				{ }
		
		//! Deinitialize the class
		void	Clear(void)			{ seconds = minutes = hours = 0; hasData = false; }
		//! Sets the class to the current local time (hours, minutes, seconds; the date is ignored)
		void	SetToNow(void)
		{
			time_t	now = ::time(NULL);
			struct tm	local;

#ifdef _WIN32
			localtime_s(&local, &now);
#else
			localtime_r(&now, &local);
#endif

			Set((UInt8)local.tm_sec, (UInt8)local.tm_min, (UInt8)local.tm_hour);
		}
		
		//! Sets the class to the specified time
		void	Set(UInt8 inS, UInt8 inM, UInt8 inH)
									{ seconds = inS; minutes = inM; hours = inH; hasData = true; }
		
		//! Gets whether the class has been initialized or not
		bool	IsSet(void)			{ return hasData; }
		
		UInt8	GetSeconds(void)	{ return seconds; }	//!< return the seconds portion of the time
		UInt8	GetMinutes(void)	{ return minutes; }	//!< return the minutes portion of the time
		UInt8	GetHours(void)		{ return hours; }	//!< return the hours portion of the time
	
	private:
		UInt8	seconds, minutes, hours;
		// hasData distinguishes "never set" from a genuine 00:00:00.
		bool	hasData;
};

// Absolute (not relative) tolerance for FloatEqual; adequate for values near 1.
const float kFloatEpsilon = 0.0001f;

/** True if a and b differ by less than kFloatEpsilon. NaN never compares equal. */
inline bool FloatEqual(float a, float b) { float magnitude = a - b; if(magnitude < 0) magnitude = -magnitude; return magnitude < kFloatEpsilon; }

/**
 * 2D float vector. Plain data (public x, y); the default constructor leaves the
 * members uninitialised. Normalize() does not guard against a zero length.
 */
class Vector2
{
	public:
		Vector2() { }
		Vector2(const Vector2 & in)					{ x = in.x; y = in.y; }
		Vector2(float inX, float inY)				{ x = inX; y = inY; }
		~Vector2() { }

		void	Set(float inX, float inY)			{ x = inX; y = inY; }
		void	SetX(float inX)						{ x = inX; }
		void	SetY(float inY)						{ y = inY; }
		void	Get(float * outX, float * outY)		{ *outX = x; *outY = y; }
		float	GetX(void)							{ return x; }
		float	GetY(void)							{ return y; }

		// Normalize divides by Magnitude(); a zero vector therefore yields NaNs.
		void	Normalize(void)						{ float mag = Magnitude(); x /= mag; y /= mag; }
		float	Magnitude(void)						{ return sqrt(x*x + y*y); }

		// Negates both components: (x,y) -> (-x,-y), the vector pointing the opposite way.
		void	Reverse(void)						{ x = -x; y = -y; }

		void	Scale(float scale)					{ x *= scale; y *= scale; }

		// Byte-swaps both components in place (see SwapFloat).
		void	SwapBytes(void)	{ SwapFloat(&x); SwapFloat(&y); }

		Vector2 &	operator+=(const Vector2 & rhs)	{ x += rhs.x; y += rhs.y; return *this; }
		Vector2 &	operator-=(const Vector2 & rhs)	{ x -= rhs.x; y -= rhs.y; return *this; }
		Vector2 &	operator*=(float rhs)			{ x *= rhs; y *= rhs; return *this; }
		Vector2 &	operator/=(float rhs)			{ x /= rhs; y /= rhs; return *this; }

		float	x;
		float	y;
};

// Free operators return new vectors; scalar * and / scale both components.
inline Vector2 operator+(const Vector2 & lhs, const Vector2 & rhs)
{
	return Vector2(lhs.x + rhs.x, lhs.y + rhs.y);
};

inline Vector2 operator-(const Vector2 & lhs, const Vector2 & rhs)
{
	return Vector2(lhs.x - rhs.x, lhs.y - rhs.y);
};

inline Vector2 operator*(const Vector2 & lhs, float rhs)
{
	return Vector2(lhs.x * rhs, lhs.y * rhs);
};

inline Vector2 operator/(const Vector2 & lhs, float rhs)
{
	return Vector2(lhs.x / rhs, lhs.y / rhs);
};

/**
 * True if lhs and rhs are equal on every bit selected by mask.
 * @param size Number of bytes to compare; all three buffers must be at least this long.
 */
inline bool MaskCompare(void * lhs, void * rhs, void * mask, UInt32 size)
{
	UInt8	* lhs8 = (UInt8 *)lhs;
	UInt8	* rhs8 = (UInt8 *)rhs;
	UInt8	* mask8 = (UInt8 *)mask;

	for(UInt32 i = 0; i < size; i++)
		if((lhs8[i] & mask8[i]) != (rhs8[i] & mask8[i]))
			return false;

	return true;
}

// Operators on Vector3 are component-wise (including * and /), not dot/cross
// products. The anonymous union lets the same storage be used as x/y/z or d[3].
/** 3D float vector; see the note above for operator semantics. Default constructor leaves members uninitialised. */
class Vector3
{
public:
	Vector3()									{ }
	Vector3(const Vector3 & in)					{ x = in.x; y = in.y; z = in.z; }
	Vector3(float inX, float inY, float inZ)	{ x = inX; y = inY; z = inZ; }
	~Vector3()									{ }

	void	Set(float inX, float inY, float inZ)			{ x = inX; y = inY; z = inZ; }
	void	Get(float * outX, float * outY, float * outZ)	{ *outX = x; *outY = y; *outZ = z; }

	void	Normalize(void)	{ float mag = Magnitude(); x /= mag; y /= mag; z /= mag; }
	float	Magnitude(void)	{ return sqrt(x*x + y*y + z*z); }

	void	Scale(float scale)	{ x *= scale; y *= scale; z *= scale; }

	void	SwapBytes(void)	{ SwapFloat(&x); SwapFloat(&y); SwapFloat(&z); }

	Vector3 &	operator+=(const Vector3 & rhs)	{ x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
	Vector3 &	operator-=(const Vector3 & rhs)	{ x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
	Vector3 &	operator*=(const Vector3 & rhs)	{ x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
	Vector3 &	operator/=(const Vector3 & rhs)	{ x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }

	union
	{
		struct
		{
			float	x, y, z;
		};
		float	d[3];
	};
};

inline Vector3 operator+(const Vector3 & lhs, const Vector3 & rhs)
{
	return Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

inline Vector3 operator-(const Vector3 & lhs, const Vector3 & rhs)
{
	return Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

inline Vector3 operator*(const Vector3 & lhs, const Vector3 & rhs)
{
	return Vector3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}

inline Vector3 operator/(const Vector3 & lhs, const Vector3 & rhs)
{
	return Vector3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
}
