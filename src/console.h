/*
 * Latest Vice City: Multiplayer (VC:MP) 0.4 features for Squirrel
 * Author: sfwidde ([SS]Kelvin)
 * 2024-07-07
 */

#ifndef CONSOLE_H
#define CONSOLE_H

#define OUTPUT_INFO(format, ...)    SQLF_OutputMessage(OUTPUT_INFO,    (format), ##__VA_ARGS__)
#define OUTPUT_WARNING(format, ...) SQLF_OutputMessage(OUTPUT_WARNING, (format), ##__VA_ARGS__)
#define OUTPUT_ERROR(format, ...)   SQLF_OutputMessage(OUTPUT_ERROR,   (format), ##__VA_ARGS__)

typedef enum
{
	OUTPUT_INFO,
	OUTPUT_WARNING,
	OUTPUT_ERROR
} OutputMessageType;

void SQLF_OutputMessage(OutputMessageType messageType, const char* format, ...);

#endif
