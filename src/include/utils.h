#ifndef INCLUDE_UTILS_H
#define INCLUDE_UTILS_H

#ifdef OS_WINDOWS
	#define LIBRARY_EXPORT __declspec(dllexport)
#else
	#define LIBRARY_EXPORT
#endif

// Returns number of elements in a fixed-size array.
// Do not use on a pointer - it could give misleading results!
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))

#endif
