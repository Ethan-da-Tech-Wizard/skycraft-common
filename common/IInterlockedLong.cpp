// ==============================================================================
// Program: IInterlockedLong.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Implementation of atomic compare-and-swap, increment, and exchange routines.
//          Inputs: Memory addresses of volatile variables and operand values.
//          Outputs: Atomic exchange results guaranteed across multi-core CPU architectures.
//          Algorithm: CPU LOCK-prefixed instructions (_InterlockedIncrement, etc.).
// ==============================================================================

#include "IInterlockedLong.h"

// all functions are inlined
