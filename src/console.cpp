/*
 * Latest Vice City: Multiplayer (VC:MP) 0.4 features for Squirrel
 * Author: sfwidde ([SS]Kelvin)
 * 2024-07-07
 */

#include "console.hpp"
#include <stdarg.h>
#ifdef OS_WINDOWS
#include <windows.h>
#endif
#include <stdio.h>

void OutputMessage(OutputMessageType messageType, const char* format, ...)
{
	// https://bitbucket.org/stormeus/0.4-squirrel/src/master/ConsoleUtils.cpp
	// Trailing spaces added for consistency with the implementation from the above link
	static constexpr const char* messagePrefixes[3] =
	{
	#ifdef OS_WINDOWS
		"[MODULE]  ",
		"[WARNING] ",
		"[ERROR]   "
	#else
		"[MODULE]",
		"[WARNING]",
		"[ERROR]"
	#endif
	};

	// Set up variadic arguments
	va_list ap;
	va_start(ap, format);

#ifdef OS_WINDOWS
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE); // Handle to the standard output device (the console)
	CONSOLE_SCREEN_BUFFER_INFO csbi; // Console's current screen buffer info
	// Did we fail to retrieve either of them?
	if (!consoleHandle || consoleHandle == INVALID_HANDLE_VALUE ||
		!GetConsoleScreenBufferInfo(consoleHandle, &csbi))
	{
		// What we needed these for is for cosmetic purposes only
		// so just output what we need to output anyway
		fputs(messagePrefixes[messageType], stdout);
		vprintf(format, ap);
		putchar('\n');
		// Cleanup
		va_end(ap);
		return;
	}

	// https://learn.microsoft.com/en-us/windows/console/console-screen-buffers#character-attributes
	static constexpr WORD messagePrefixColorAttributes[3] =
	{
		FOREGROUND_GREEN,                                         // Green
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, // Bright yellow
		FOREGROUND_RED | FOREGROUND_INTENSITY                     // Bright red
	};

	// Color message prefix accordingly
	SetConsoleTextAttribute(consoleHandle, messagePrefixColorAttributes[messageType]);
	fputs(messagePrefixes[messageType], stdout);
	// Official VC:MP plugins' console messages are completely white for Windows builds so we follow this convention too
	SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	vprintf(format, ap);
	// Reset console's color to its initial state for any future messages not related to this plugin
	SetConsoleTextAttribute(consoleHandle, csbi.wAttributes);
	putchar('\n');
#else
	// https://en.wikipedia.org/wiki/ANSI_escape_code#3-bit_and_4-bit
	static constexpr int messagePrefixColorCodes[3] =
	{
		32, // Green
		93, // Bright yellow
		91  // Bright red
	};

	printf("\x1B[%dm%s\x1B[0m ", messagePrefixColorCodes[messageType], messagePrefixes[messageType]);
	vprintf(format, ap);
	putchar('\n');
#endif

	// Cleanup
	va_end(ap);
}
