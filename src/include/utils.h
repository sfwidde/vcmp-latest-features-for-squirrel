/*
 * Latest Vice City: Multiplayer (VC:MP) 0.4 features for Squirrel
 * Author: sfwidde ([SS]Kelvin)
 * 2024-07-07
 */

#ifndef INCLUDE_UTILS_H
#define INCLUDE_UTILS_H

#ifdef OS_WINDOWS
	#define LIBRARY_EXPORT __declspec(dllexport)
#else
	#define LIBRARY_EXPORT
#endif

typedef unsigned char BYTE;

// Return number of elements in an array
// Use on fixed-size arrays only -- doing otherwise could give misleading results!
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))
// Add padding to structure
#define PAD(x, n) const BYTE pad ## x[n]

#endif
