/*
 * Latest Vice City: Multiplayer (VC:MP) 0.4 features for Squirrel
 * Author: sfwidde ([SS]Kelvin)
 * 2024-07-07
 */

#ifndef INCLUDE_UTILS_HPP
#define INCLUDE_UTILS_HPP

#ifdef OS_WINDOWS // Windows
	#define LIBRARY_EXPORT __declspec(dllexport)
#else             // Linux
	#define LIBRARY_EXPORT
#endif

#define BEGIN_C_LINKAGE extern "C" {
#define END_C_LINKAGE   }

// Add padding to structure
#define PAD(x, n) BYTE pad ## x[n]

typedef unsigned char BYTE;

#endif
