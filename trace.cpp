#include "trace.h"

#include <Windows.h>
#include <stdio.h>

void _cdecl printDebug(const char* message, ...) {
#ifdef _DEBUG
	va_list	arg;
	va_start(arg, message);
	vprintf(message, arg);
	va_end(arg);
#endif // _DEBUG
}