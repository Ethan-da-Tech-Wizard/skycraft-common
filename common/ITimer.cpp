// ==============================================================================
// Program: ITimer.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Implementation of high-resolution timing, frequency caching, and elapsed intervals.
//          Inputs: QueryPerformanceFrequency calibration.
//          Outputs: Precise delta time calculations.
//          Algorithm: 64-bit integer timestamp subtraction divided by clock frequency.
// ==============================================================================


#include "ITimer.h"

// Design: elapsed time normally comes from the high-resolution counter (QPC),
// but QPC is cross-checked against the coarse multimedia timer (timeGetTime) to
// catch hardware/driver glitches; the statics below hold state shared by all
// ITimer instances.
// QueryPerformanceCounter is very accurate, but hardware bugs can cause it to return inaccurate results
// this code uses multimedia timers to check for glitches in QPC

// Shared state, one copy for all ITimer objects (they all read the same hardware counter):
// s_secondsPerCount = 1 / QPC frequency; s_timecaps = multimedia timer limits; s_setTime = whether
// timeBeginPeriod succeeded (so DeInit knows to undo it); s_lastQPC / s_hasLastQPC = previous QPC sample used
// for the monotonic check; s_qpcWrapMargin = backwards-jump threshold; s_qpcWrapCount / s_qpcInaccurateCount = diagnostics.
double		ITimer::s_secondsPerCount = 0;
TIMECAPS	ITimer::s_timecaps = { 0 };
bool		ITimer::s_setTime = false;
UInt64		ITimer::s_lastQPC = 0;
UInt64		ITimer::s_qpcWrapMargin = 0;
bool		ITimer::s_hasLastQPC = false;
UInt32		ITimer::s_qpcWrapCount = 0;
UInt32		ITimer::s_qpcInaccurateCount = 0;

/// Initialise the shared timer state (once per process) and zero this timer's base values; call Start() before reading elapsed time.
ITimer::ITimer()
:m_qpcBase(0), m_tickBase(0)
{
	Init();
}

/// Nothing per-instance to release; call DeInit() once at shutdown to restore the multimedia timer period.
ITimer::~ITimer()
{

}

// One-time setup (guarded by s_secondsPerCount): read the QPC frequency, compute
// the wrap-detection margin, and raise the multimedia timer resolution to its
// minimum period. Each Init is paired with a DeInit that restores it.
void ITimer::Init(void)
{
	if(!s_secondsPerCount)
	{
		// init qpc: counts per second of the high-resolution counter (typically ~10 MHz). Its absence is fatal because everything else depends on it.
		// init qpc
		UInt64	countsPerSecond;
		BOOL res = QueryPerformanceFrequency((LARGE_INTEGER *)&countsPerSecond);

		ASSERT_STR(res, "ITimer: no high-resolution timer support");

		s_secondsPerCount = 1.0 / countsPerSecond;

		// A "backwards" step shows up as a huge unsigned delta; any delta above
		// -(60 seconds worth of counts) is treated as time moving backwards.
		s_qpcWrapMargin = (UInt64)(-((SInt64)(countsPerSecond * 60)));	// detect if we've wrapped around by a delta greater than this - also limits max time
		_MESSAGE("s_qpcWrapMargin: %016I64X", s_qpcWrapMargin);
		_MESSAGE("wrap time: %fs", ((double)0xFFFFFFFFFFFFFFFF) * s_secondsPerCount);

		// init multimedia timer: ask for the finest period the system supports (usually 1 ms) so timeGetTime() is precise enough to cross-check QPC.
		// init multimedia timer
		timeGetDevCaps(&s_timecaps, sizeof(s_timecaps));

		_MESSAGE("min timer period = %d", s_timecaps.wPeriodMin);

		s_setTime = (timeBeginPeriod(s_timecaps.wPeriodMin) == TIMERR_NOERROR);
		if(!s_setTime)
			_WARNING("couldn't change timer period");
	}
}

/// Undo Init(): restore the timer period and reset state so a later Init() starts clean. Call once at shutdown.
void ITimer::DeInit(void)
{
	if(s_secondsPerCount)
	{
		if(s_setTime)
		{
			timeEndPeriod(s_timecaps.wPeriodMin);
			s_setTime = false;
		}

		if(s_qpcWrapCount)
			_MESSAGE("s_qpcWrapCount: %d", s_qpcWrapCount);

		s_secondsPerCount = 0;
	}
}


// ---------------------------------------------------------------------------
// Start: Samples initial timestamp using Win32 QueryPerformanceCounter
// ---------------------------------------------------------------------------
void ITimer::Start(void)
{
	m_qpcBase = GetQPC();
	m_tickBase = timeGetTime();
}

/**
 * Seconds since Start(). If the QPC and multimedia-timer readings disagree by
 * more than one second, QPC is considered unreliable and the (millisecond)
 * multimedia value is returned instead; the event is counted.
 *
 * @return Elapsed time in seconds (double). The tick delta is computed in 32-bit milliseconds and wraps after ~49 days.
 */
// Seconds since Start(). If the QPC and multimedia-timer readings disagree by
// more than one second, QPC is considered unreliable and the (millisecond)
// multimedia value is returned instead; the event is counted.
double ITimer::GetElapsedTime(void)
{
	UInt64	qpcNow = GetQPC();
	UInt32	tickNow = timeGetTime();

	// Unsigned subtraction keeps the deltas right across counter wrap.
	UInt64	qpcDelta = qpcNow - m_qpcBase;
	UInt64	tickDelta = tickNow - m_tickBase;

	double	qpcSeconds = ((double)qpcDelta) * s_secondsPerCount;
	double	tickSeconds = ((double)tickDelta) * 0.001;	// ticks are in milliseconds
	double	qpcTickDelta = qpcSeconds - tickSeconds;

	if(qpcTickDelta < 0) qpcTickDelta = -qpcTickDelta;

	// if they differ by more than one second, something's wrong, return
	// if they differ by more than one second, something's wrong, return
	if(qpcTickDelta > 1)
	{
		s_qpcInaccurateCount++;
		return tickSeconds;
	}
	else
	{
		return qpcSeconds;
	}
}

/**
 * Reads QPC and enforces monotonicity: if the counter appears to jump backwards
 * the value is replaced by last+1 and the event is counted.
 *
 * @return Raw high-resolution counter value (counts, not seconds).
 */
// Reads QPC and enforces monotonicity: if the counter appears to jump backwards
// the value is replaced by last+1 and the event is counted.
UInt64 ITimer::GetQPC(void)
{
	UInt64	now;

	QueryPerformanceCounter((LARGE_INTEGER *)&now);

	if(s_hasLastQPC)
	{
		// Unsigned delta: a backwards step appears as a huge positive number, above the margin computed in Init().
		UInt64	delta = now - s_lastQPC;

		if(delta > s_qpcWrapMargin)
		{
			// we've gone back in time, return a kludged value
			// we've gone back in time, return a kludged value

			s_lastQPC = now;
			now = s_lastQPC + 1;

			s_qpcWrapCount++;
		}
		else
		{
			s_lastQPC = now;
		}
	}
	else
	// First sample: nothing to compare with yet.
	{
		s_hasLastQPC = true;
		s_lastQPC = now;
	}

	return now;
}
