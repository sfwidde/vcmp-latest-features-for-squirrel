#ifndef PLUGIN_CONSOLE_H
#define PLUGIN_CONSOLE_H

#define OUTPUT_INFO(format, ...)    OutputMessage(OUTPUT_INFO,    (format), ##__VA_ARGS__)
#define OUTPUT_WARNING(format, ...) OutputMessage(OUTPUT_WARNING, (format), ##__VA_ARGS__)
#define OUTPUT_ERROR(format, ...)   OutputMessage(OUTPUT_ERROR,   (format), ##__VA_ARGS__)

typedef enum
{
	OUTPUT_INFO,
	OUTPUT_WARNING,
	OUTPUT_ERROR
} OutputMessageType;

void OutputMessage(OutputMessageType messageType, const char* format, ...);

#endif
