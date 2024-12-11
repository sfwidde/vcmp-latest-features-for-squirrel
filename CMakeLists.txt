# Latest Vice City: Multiplayer (VC:MP) 0.4 features for Squirrel
# Author: sfwidde ([SS]Kelvin)
# 2024-07-07

cmake_minimum_required(VERSION 3.12)

project(vcmp-latest-features-for-squirrel)

set(BUILD_SHARED_LIBS ON)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
math(EXPR CPU_BITS "${CMAKE_SIZEOF_VOID_P} * 8")

# Windows
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	# Follow VC:MP plugin naming convention (04rel32/64.dll)
	set(CMAKE_SHARED_LIBRARY_SUFFIX "04rel${CPU_BITS}.dll")

	add_compile_definitions(OS_WINDOWS)
	add_compile_definitions(WIN32_LEAN_AND_MEAN) # For files that include <Windows.h>
# Linux
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	# Follow VC:MP plugin naming convention (04rel32/64.so)
	set(CMAKE_SHARED_LIBRARY_SUFFIX "04rel${CPU_BITS}.so")

	add_compile_definitions(OS_LINUX)
else()
	message(FATAL_ERROR "${CMAKE_SYSTEM_NAME} is an unsupported operating system. Use Windows or Linux instead")
endif()

# Microsoft Visual C++
if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
	add_compile_definitions(COMP_MSVC)
	# Suppress some C4996 warnings
	# https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-3-c4996?view=msvc-170#disable-the-warning-using-preprocessor-macros
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS)
# GNU Compiler Collection
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
	add_compile_definitions(COMP_GCC)
else()
	message(WARNING "MSVC and GCC are tested compilers whereas ${CMAKE_C_COMPILER_ID} is not. "
		"Code might function in unexpected ways or it may not even compile at all")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_definitions(DEBUG_BUILD=1)
else()
	add_compile_definitions(DEBUG_BUILD=0)
endif()

include_directories(src/include/)

add_subdirectory(src/)
