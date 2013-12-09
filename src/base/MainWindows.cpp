#if defined(WIN32)

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>
#include "Main.h"

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	return usdxMain();
}

#endif
