/*
 * Latest Vice City: Multiplayer (VC:MP) 0.4 features for Squirrel
 * Author: sfwidde ([SS]Kelvin)
 * 2024-07-07
 */

#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#define OUTPUT_INFO(format, ...)    OutputMessage(OUTPUT_INFO,    (format), ##__VA_ARGS__)
#define OUTPUT_WARNING(format, ...) OutputMessage(OUTPUT_WARNING, (format), ##__VA_ARGS__)
#define OUTPUT_ERROR(format, ...)   OutputMessage(OUTPUT_ERROR,   (format), ##__VA_ARGS__)

enum OutputMessageType
{
	OUTPUT_INFO,
	OUTPUT_WARNING,
	OUTPUT_ERROR
};

void OutputMessage(OutputMessageType messageType, const char* format, ...);

#endif
