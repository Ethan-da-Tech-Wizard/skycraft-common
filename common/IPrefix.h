// ==============================================================================
// Program: IPrefix.h
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: String prefix matching, token trie, and string classification utility header.
//          Inputs: Text strings, candidate prefixes, and case-sensitivity flags.
//          Outputs: Match boolean results and stripped suffix substrings.
//          Architecture: Command and configuration key parsing for SKSE64.
// ==============================================================================

#pragma once

// 4018 - signed/unsigned mismatch
// 4200 - zero-sized array
// 4244 - loss of data by assignment
// 4267 - possible loss of data (truncation)
// 4305 - truncation by assignment
// 4288 - disable warning for crap microsoft extension screwing up the scope of variables defined in for loops
// 4311 - pointer truncation
// 4312 - pointer extension
#pragma warning(disable: 4018 4200 4244 4267 4305 4288 4312 4311)

// need win8 for windows store APIs
#define _WIN32_WINNT	0x0602

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include "common/ITypes.h"
#include "common/IErrors.h"
#include "common/IDynamicCreate.h"
#include "common/IDebugLog.h"
#include "common/ISingleton.h"
#include <winsock2.h>
#include <Windows.h>
