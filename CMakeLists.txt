# Latest Vice City: Multiplayer (VC:MP) 0.4 features for Squirrel
# Author: sfwidde ([SS]Kelvin)
# 2024-07-07

cmake_minimum_required(VERSION 3.21)

project(vcmp-latest-features-for-squirrel)

# Set and enforce C++11 as the standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_SHARED_LIBS ON) # Build as DLL/SO by default
set(CMAKE_SHARED_LIBRARY_PREFIX "")
math(EXPR CPU_BITS "${CMAKE_SIZEOF_VOID_P} * 8")

# Windows
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	# Follow VC:MP plugin naming convention (04rel32/64.dll)
	set(CMAKE_SHARED_LIBRARY_SUFFIX "04rel${CPU_BITS}.dll")

	add_compile_definitions(OS_WINDOWS)
	# For files that include <windows.h>:
	# - A value of 0x0601 for WINVER and _WIN32_WINNT defines Windows 7
	# as the least supported version of Windows
	add_compile_definitions(WINVER=0x0601 _WIN32_WINNT=0x0601 WIN32_LEAN_AND_MEAN)
# Linux
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	# Keep symbols hidden unless explicitly marked as public in code
	set(CMAKE_C_VISIBILITY_PRESET hidden)
	set(CMAKE_CXX_VISIBILITY_PRESET hidden)
	set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
	# Follow VC:MP plugin naming convention (04rel32/64.so)
	set(CMAKE_SHARED_LIBRARY_SUFFIX "04rel${CPU_BITS}.so")

	add_compile_definitions(OS_LINUX)
	add_compile_definitions(_GNU_SOURCE)
else()
	message(FATAL_ERROR "${CMAKE_SYSTEM_NAME} is an unsupported operating system. Use Windows or Linux instead")
endif()

# Microsoft Visual C++
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_compile_options(/Zc:__cplusplus)
	add_compile_definitions(COMP_MSVC)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS) # Suppress some C4996 warnings
# GNU Compiler Collection
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	add_compile_definitions(COMP_GCC)
else()
	message(WARNING "MSVC and GCC are tested compilers whereas ${CMAKE_CXX_COMPILER_ID} is not. "
		"Code might function in unexpected ways or it may not even compile at all")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_definitions(DEBUG_BUILD)
endif()

add_compile_definitions(CPU_${CPU_BITS})

include_directories(src/include/)

add_subdirectory(src/)
