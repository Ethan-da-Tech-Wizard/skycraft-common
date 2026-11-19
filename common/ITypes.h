// ==============================================================================
// Program: ITypes.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Standardized fixed-width integer and primitive type definitions.
//          Inputs: Platform architecture target (x86_64).
//          Outputs: Sized integer types (UInt8, SInt8, UInt16, SInt16, UInt32, SInt32, UInt64, SInt64).
//          Architecture: Guarantees exact binary memory layout matching game engine structs.
// ==============================================================================

#pragma once

#include "common/IErrors.h"

// MSVC-only pragma that silences warning C4221 (initializing an aggregate with
// the address of an automatic variable); other compilers ignore it.
#pragma warning(disable: 4221)
#include <cmath>

// Fixed-width unsigned and signed 8-bit integer types
// The typedefs below define the sized types used throughout the SKSE sources
// (this block covers 8-16-32-64 bit signed/unsigned integers and float/double).
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

// Sign-extend a 16-bit value held in the low bits of a 32-bit word: if bit 15 is
// set, fill the upper 16 bits with ones.
inline UInt32 Extend16(UInt32 in)
{
	return (in & 0x8000) ? (0xFFFF0000 | in) : in;
}

// Same as Extend16 for an 8-bit value.
inline UInt32 Extend8(UInt32 in)
{
	return (in & 0x80) ? (0xFFFFFF00 | in) : in;
}

// Byte-order helpers: reverse the bytes of 16/32/64-bit integers (used when
// reading data written with the opposite endianness).
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

// Swaps each 32-bit half and exchanges the halves.
inline UInt64 Swap64(UInt64 in)
{
	UInt64	temp;

	temp = Swap32(in);
	temp <<= 32;
	temp |= Swap32(in >> 32);

	return temp;
}

// In-place byte swap of a float/double by treating its bits as an integer.
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

// Runtime endianness test: store 0x1234 and check which byte comes first.
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

inline bool IsLittleEndian(void)
{
	return !IsBigEndian();
}

// Packs four characters into a 32-bit value, first character in the lowest
// byte (so the in-memory byte order spells the string on little-endian CPUs).
#define CHAR_CODE(a, b, c, d)	(((a & 0xFF) << 0) | ((b & 0xFF) << 8) | ((c & 0xFF) << 16) | ((d & 0xFF) << 24))
// Compile-time (constant expression) byte swaps, usable in initialisers.
#define MACRO_SWAP16(a)			((((a) & 0x00FF) << 8) | (((a) & 0xFF00) >> 8))
#define MACRO_SWAP32(a)			((((a) & 0x000000FF) << 24) | (((a) & 0x0000FF00) << 8) | (((a) & 0x00FF0000) >> 8) | (((a) & 0xFF000000) >> 24))

// Packs a version as 12 bits major | 12 bits minor | 8 bits sub-version (bits
// 31..20, 19..8, 7..0); the following macros extract each field.
#define VERSION_CODE(primary, secondary, sub)	(((primary & 0xFFF) << 20) | ((secondary & 0xFFF) << 8) | ((sub & 0xFF) << 0))
#define VERSION_CODE_PRIMARY(in)				((in >> 20) & 0xFFF)
#define VERSION_CODE_SECONDARY(in)				((in >> 8) & 0xFFF)
#define VERSION_CODE_SUB(in)					((in >> 0) & 0xFF)

// Packs ARGB into one 32-bit value, 8 bits per channel (alpha in the top byte).
#define MAKE_COLOR(a, r, g, b)	(((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | ((b & 0xFF) << 0))
#define COLOR_ALPHA(in)			((in >> 24) & 0xFF)
#define COLOR_RED(in)			((in >> 16) & 0xFF)
#define COLOR_GREEN(in)			((in >> 8) & 0xFF)
#define COLOR_BLUE(in)			((in >> 0) & 0xFF)

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
 */
template <typename T>
// Typed set of flag helpers over an integer `field`. Note that the mask
// parameters are UInt32, so for Bitfield64-sized use the upper bits are not
// reachable. ExtractField(shift, length) assumes length <= 32.
class Bitfield
{
	public:
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
 *	Essentially a long bitvector.
 */
class Bitstring
{
	public:
				Bitstring();
				Bitstring(UInt32 inLength);
				~Bitstring();

		void	Alloc(UInt32 inLength);
		void	Dispose(void);

		void	Clear(void);
		void	Clear(UInt32 idx);
		void	Set(UInt32 idx);

		bool	IsSet(UInt32 idx);
		bool	IsClear(UInt32 idx);

	private:
		UInt8	* data;
		UInt32	length;	//!< length in bytes
};

/**
 *	Time storage
 */
// Simple hour/minute/second holder. NOTE: SetToNow() is an unfinished stub (it
// just stores 1:02:03) - it does not read the clock.
class Time
{
	public:
				Time()				{ Clear(); }
				~Time()				{ }
		
		//! Deinitialize the class
		void	Clear(void)			{ seconds = minutes = hours = 0; hasData = false; }
		//! Sets the class to the current time
		//! @todo implement this
		void	SetToNow(void)		{ Set(1, 2, 3); }
		
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
		bool	hasData;
};

// Tolerance for FloatEqual; absolute (not relative) comparison.
const float kFloatEpsilon = 0.0001f;

inline bool FloatEqual(float a, float b) { float magnitude = a - b; if(magnitude < 0) magnitude = -magnitude; return magnitude < kFloatEpsilon; }

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

		void	Normalize(void)						{ float mag = Magnitude(); x /= mag; y /= mag; }
		float	Magnitude(void)						{ return sqrt(x*x + y*y); }

		// Despite the name, this rotates the vector by 90 degrees ((x,y) -> (-y,x)),
		// it does not negate it.
		void	Reverse(void)						{ float temp = -x; x = -y; y = temp; }

		void	Scale(float scale)					{ x *= scale; y *= scale; }

		void	SwapBytes(void)	{ SwapFloat(&x); SwapFloat(&y); }

		Vector2 &	operator+=(const Vector2 & rhs)	{ x += rhs.x; y += rhs.y; return *this; }
		Vector2 &	operator-=(const Vector2 & rhs)	{ x -= rhs.x; y -= rhs.y; return *this; }
		Vector2 &	operator*=(float rhs)			{ x *= rhs; y *= rhs; return *this; }
		Vector2 &	operator/=(float rhs)			{ x /= rhs; y /= rhs; return *this; }

		float	x;
		float	y;
};

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

// True if lhs and rhs are equal on every bit selected by mask (`size` bytes).
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
