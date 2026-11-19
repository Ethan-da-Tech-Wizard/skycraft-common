#!/bin/bash
# Builds and runs tests/test_itypes.cpp on a POSIX machine.
#
# The SKSE headers assume the Windows ABI where `long` is 32 bits. On macOS and
# Linux `long` is 64 bits, which breaks the STATIC_ASSERTs in ITypes.h, so this
# script works on a temporary copy in which the UInt32/SInt32 typedefs use `int`.
# The repository sources are not modified. Requires clang++ or g++.
set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
work="$(mktemp -d)"
trap 'rm -rf "$work"' EXIT

cp -R "$root/common" "$work/common"
# portable in-place edit (BSD and GNU sed differ), so use a temp file
python3 - "$work/common/ITypes.h" <<'PY'
import re, sys
path = sys.argv[1]
text = open(path).read()
text = re.sub(r"typedef unsigned long(\s+)UInt32;", r"typedef unsigned int\1UInt32;", text)
text = re.sub(r"typedef signed long(\s+)SInt32;", r"typedef signed int\1SInt32;", text)
open(path, "w").write(text)
PY

cxx="${CXX:-c++}"
"$cxx" -std=c++14 -Wall -Wno-unknown-pragmas ${EXTRA_CXXFLAGS:-} -I"$work" "$root/tests/test_itypes.cpp" "$work/common/ITypes.cpp" -o "$work/test_itypes"
"$work/test_itypes"
