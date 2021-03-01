#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif // !WIN32_LEAN_AND_MEAN

#ifndef KDEXT_64BIT
#define KDEXT_64BIT
#endif // !KDEXT_64BIT

#include <windows.h>
#include <DbgEng.h>
#include <string>
#include <sstream>
#include <WDBGEXTS.H>

#include "callbacks.h"

#define MAJOR	1
#define MINOR	0
#define MAX_SYMBOL_LENGTH	256


/// EOF