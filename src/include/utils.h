#ifndef INCLUDE_UTILS_H
#define INCLUDE_UTILS_H

#ifdef OS_WINDOWS
	#define LIBRARY_EXPORT __declspec(dllexport)
#else
	#define LIBRARY_EXPORT
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))

#endif
