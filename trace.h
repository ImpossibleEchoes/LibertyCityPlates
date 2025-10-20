#pragma once

void _cdecl printDebug(const char* message, ...);


#ifdef _DEBUG
#define PRINT_DUBUG(format, ...) \
	printDebug(format, ##__VA_ARGS__)
#else
#define PRINT_DUBUG(format, ...) ;
#endif // _DEBUG
