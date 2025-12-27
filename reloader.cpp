#include "reloader.h"

#include "addrs.h"
#include "hookFns.h"

#include "config.h"
#include "trace.h"

#define KEY_0 11
#define KEY_K 37
#define KEY_J 36

bool isGameKeyboardKeyPressed(int key) {
	int* context[3];
	context[2] = &key;
	int result;
	context[0] = &result;
	((int(__cdecl*)(void*))(g_nativeCallAddr_isGameKeyboardKeyPressed))(context);

	return result;
}

bool isGameKeyboardKeyJustPressed(int key) {
	int* context[3];
	context[2] = &key;
	int result;
	context[0] = &result;
	((int(__cdecl*)(void*))(g_nativeCallAddr_isGameKeyboardKeyJustPressed))(context);

	return result;
}

size_t g_processGame_origcall;

void dumpIdeData();
void readIdeData();


char processGame() {
	if (isGameKeyboardKeyPressed(KEY_0) && isGameKeyboardKeyJustPressed(KEY_K))
		dumpIdeData();
	else if (isGameKeyboardKeyPressed(KEY_0) && isGameKeyboardKeyJustPressed(KEY_J))
		readIdeData();

	return ((char(*)())(g_processGame_origcall))();

}

void initReloader() {
	if (CConfig::ms_bImportExportData) {
		g_processGame_origcall = setFnAddrInCallOpcode(g_hookAddr_CWorld__process, (size_t)processGame);
		PRINT_DUBUG("[initReloader(...)] enabled\n");
	}
}

