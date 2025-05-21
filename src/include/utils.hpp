/*
 * Latest Vice City: Multiplayer (VC:MP) 0.4 features for Squirrel
 * Author: sfwidde ([SS]Kelvin)
 * 2024-07-07
 */

#ifndef INCLUDE_UTILS_HPP
#define INCLUDE_UTILS_HPP

#ifdef OS_WINDOWS
	#define LIBRARY_EXPORT __declspec(dllexport)
#else // OS_LINUX
	#define LIBRARY_EXPORT __attribute__((visibility("default")))
#endif

#define C_LINKAGE_BEGIN extern "C" {
#define C_LINKAGE_END   }

// Add padding to structure
#define PAD(x, n) BYTE pad ## x[n]

typedef unsigned char BYTE;

#endif
