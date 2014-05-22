/*
	Stock entry point for DLLs that don't otherwise have them...
	e.g. zlib
*/

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	hinstDLL;
	fdwReason;
	lpvReserved;
	return TRUE;
}
