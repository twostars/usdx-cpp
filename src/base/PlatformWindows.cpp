/* UltraStar Deluxe - Karaoke Game
 *
 * UltraStar Deluxe is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING. If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */           

#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <Windows.h>
#include "Platform.h"
#include "Main.h"

int APIENTRY _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	BOOL attachedConsole = AttachConsole(ATTACH_PARENT_PROCESS);

#if defined(_DEBUG)
	// Clear the console.
	if (attachedConsole)
		system("cls");
	// Not attached to a console, so create one.
	else
		AllocConsole();
#endif

	// Redirect the input/output streams to the console.
	freopen("CON", "r", stdin);
	freopen("CON", "w", stdout);
	freopen("CON", "w", stderr);

	int result = usdxMain(__argc, __targv);

#if defined(_DEBUG)
	// blocks, so advise user what's going on.
	if (attachedConsole)
		_tprintf(_T("Please press any key to continue.\n"));
	// produce equivalent blocking behaviour for our console.
	else
		system("pause");
#endif

	FreeConsole();
	return result;
}

bool Platform::TerminateIfAlreadyRunning(const wchar_t * windowTitle)
{
	HWND hWnd = FindWindowW(NULL, windowTitle);
	if (hWnd == NULL)
		return false;

	// TODO: Langify the message.
	int dialogResult = MessageBoxW(NULL, L"Another instance of UltraStar is already running. Continue?", windowTitle, MB_ICONWARNING | MB_YESNO);

	// Don't bother creating another instance, just exit.
	if (dialogResult != IDYES)
		return true;

	// TODO: Allow for multiple instances with different window titles.
	// i.e. <title> Instance <number>

	// No other instance exists.
	return false;
}
