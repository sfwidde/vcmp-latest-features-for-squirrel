#ifndef CONSOLE_H
#define CONSOLE_H

#define SQLF_OUTPUT_INFO(format, ...)    SQLF_OutputMessage(SQLF_OUTPUT_INFO,    (format), ##__VA_ARGS__)
#define SQLF_OUTPUT_WARNING(format, ...) SQLF_OutputMessage(SQLF_OUTPUT_WARNING, (format), ##__VA_ARGS__)
#define SQLF_OUTPUT_ERROR(format, ...)   SQLF_OutputMessage(SQLF_OUTPUT_ERROR,   (format), ##__VA_ARGS__)

typedef enum
{
	SQLF_OUTPUT_INFO,
	SQLF_OUTPUT_WARNING,
	SQLF_OUTPUT_ERROR
} SQLF_OutputMessageType;

void SQLF_OutputMessage(SQLF_OutputMessageType messageType, const char* format, ...);

#endif
