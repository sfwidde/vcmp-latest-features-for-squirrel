#include "console.h"
#include <stdarg.h>
#ifdef OS_WINDOWS
#include <Windows.h>
#endif
#include <stdio.h>

void OutputMessage(OutputMessageType messageType, const char* format, ...)
{
	static const char* const messagePrefixes[3] =
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

	va_list ap;
	va_start(ap, format);

#ifdef OS_WINDOWS
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!consoleHandle || consoleHandle == INVALID_HANDLE_VALUE ||
		!GetConsoleScreenBufferInfo(consoleHandle, &csbi))
	{
		fputs(messagePrefixes[messageType], stdout);
		vprintf(format, ap);
		putchar('\n');

		va_end(ap);
		return;
	}

	// https://learn.microsoft.com/en-us/windows/console/console-screen-buffers#character-attributes
	static const WORD messagePrefixColorAttributes[3] =
	{
		FOREGROUND_GREEN,                                         // Green
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, // Bright yellow
		FOREGROUND_RED |                    FOREGROUND_INTENSITY  // Bright red
	};

	SetConsoleTextAttribute(consoleHandle, messagePrefixColorAttributes[messageType]);
	fputs(messagePrefixes[messageType], stdout);

	SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	vprintf(format, ap);

	SetConsoleTextAttribute(consoleHandle, csbi.wAttributes);
	putchar('\n');
#else
	// https://en.wikipedia.org/wiki/ANSI_escape_code#3-bit_and_4-bit
	static const int messagePrefixColorCodes[3] =
	{
		32, // Green
		93, // Bright yellow
		91  // Bright red
	};

	printf("\x1B[%dm%s\x1B[0m ", messagePrefixColorCodes[messageType], messagePrefixes[messageType]);
	vprintf(format, ap);
	putchar('\n');
#endif

	va_end(ap);
}
