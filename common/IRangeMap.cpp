// ==============================================================================
// Program: IRangeMap.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Implementation of address range interval insertions and point queries.
//          Inputs: Address intervals [base, base + length) and query addresses.
//          Outputs: Matching range descriptors or null.
//          Algorithm: Ordered tree search with non-overlapping range verification.
// ==============================================================================

#include "IRangeMap.h"
