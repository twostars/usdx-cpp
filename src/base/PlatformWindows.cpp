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

#include <ShlObj.h>
#pragma comment(lib, "Shell32.lib")

#include "Platform.h"
#include "Main.h"

using namespace boost::filesystem;

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
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

	int result = usdxMain(__argc, __argv);

#if defined(_DEBUG)
	// blocks, so advise user what's going on.
	if (attachedConsole)
		printf("Please press any key to continue.\n");
#endif

	FreeConsole();
	return result;
}

bool Platform::TerminateIfAlreadyRunning(const char * windowTitle)
{
	HWND hWnd = FindWindowA(NULL, windowTitle);
	if (hWnd == NULL)
		return false;

	int dialogResult = MessageBoxA(NULL,
		"Another instance of UltraStar is already running. Continue?",
		windowTitle, MB_ICONWARNING | MB_YESNO);

	// Don't bother creating another instance, just exit.
	if (dialogResult != IDYES)
		return true;

	// No other instance exists.
	return false;
}

void Platform::ShowMessage(const char * message, MessageType messageType)
{
	uint32 flags = 0;
	switch (messageType)
	{
	case mtInfo:
		flags |= MB_ICONINFORMATION | MB_OK;
		break;

	case mtError:
		flags |= MB_ICONERROR | MB_OK;
		break;

	default:
		flags |= MB_OK;
		break;
	}

	MessageBoxA(NULL, message, USDXVersionStr(), flags);
}

/**
 * Detects whether the was executed locally or globally.
 * - Local mode:
 *   - Condition:
 *     - config.ini is writable or creatable in the directory of the executable.
 *   - Examples:
 *     - The USDX zip-archive has been unpacked to a directory with write.
 *       permissions
 *     - XP: USDX was installed to %ProgramFiles% and the user is an admin.
 *     - USDX is started from an external HD- or flash-drive
 *   - Behavior:
 *     Config files like config.ini or score db reside in the directory of the
 *     executable. This is useful to enable windows users to have a portable
 *     installation e.g. on an external hdd.
 *     This is also the default behaviour of usdx prior to version 1.1
 * - Global mode:
 *   - Condition:
 *     - config.ini is not writable.
 *   - Examples:
 *     - Vista/7: USDX was installed to %ProgramFiles%.
 *     - XP: USDX was installed to %ProgramFiles% and the user is not an admin.
 *     - USDX is started from CD
 *   - Behavior:
 *     - The config files are in a separate folder (e.g. %APPDATA%\ultrastardx)
 *
 * On Windows, resources (themes, language-files)
 * reside in the directory of the executable in any case
 *
 * Sets UseLocalDirs to true if the game is executed locally, false otherwise.
 */
void Platform::DetectLocalExecution()
{
	path configFile;
	GetExecutionDir(&configFile);
	configFile /= CONFIG_FILE;

	// Can the config file be opened? 
	// Create it if it doesn't exist.
	FILE * fp = fopen(configFile.generic_string().c_str(), "a");
	if (fp != NULL)
	{
		s_useLocalDirs = true;
		fclose(fp);
	}
}

/**
 * Returns the path of a special folder.
 *
 * Some Folder IDs:
 * CSIDL_APPDATA       (e.g. C:\Documents and Settings\username\Application Data)
 * CSIDL_LOCAL_APPDATA (e.g. C:\Documents and Settings\username\Local Settings\Application Data)
 * CSIDL_PROFILE       (e.g. C:\Documents and Settings\username)
 * CSIDL_PERSONAL      (e.g. C:\Documents and Settings\username\My Documents)
 * CSIDL_MYMUSIC       (e.g. C:\Documents and Settings\username\My Documents\My Music)
 */
void Platform::GetSpecialPath(int csidl, path * outPath)
{
	char path[_MAX_PATH] = {0};
	if (SUCCEEDED(SHGetFolderPath(NULL, csidl, NULL, 0, (LPTSTR) &path)))
		*outPath = path;
}

void Platform::GetLogPath(path * outPath)
{
	GetGameUserPath(outPath);
}

void Platform::GetGameSharedPath(path * outPath)
{
	GetExecutionDir(outPath);
}

void Platform::GetGameUserPath(path * outPath)
{
	if (s_useLocalDirs)
		return GetExecutionDir(outPath);

	GetSpecialPath(CSIDL_APPDATA, outPath);
	
	*outPath /= USDX_IDENTIFIER;
}

void Platform::GetMusicPath(path * outPath)
{
	GetGameSharedPath(outPath);
}

/*
	Checks access rights to the specified file/folder 
	Author: Aaron Ballman
	http://blog.aaronballman.com/2011/08/how-to-check-access-rights/
*/ 
bool HasAccessRights(LPCSTR name, DWORD genericAccessRights)
{
	bool bRet = false;
	DWORD length = 0;
	SECURITY_INFORMATION requestedInformation 
		= OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION;

	if (!GetFileSecurityA(name, requestedInformation, NULL, NULL, &length)
		&& ERROR_INSUFFICIENT_BUFFER == GetLastError())
	{
		PSECURITY_DESCRIPTOR security = static_cast<PSECURITY_DESCRIPTOR>(malloc(length));
		if (security != NULL
			&& GetFileSecurityA(name, requestedInformation, security, length, &length))
		{
			HANDLE hToken = NULL;
			if (OpenProcessToken(
				GetCurrentProcess(), 
				TOKEN_IMPERSONATE | TOKEN_QUERY | TOKEN_DUPLICATE | STANDARD_RIGHTS_READ, 
				&hToken))
			{
				HANDLE hImpersonatedToken = NULL;
				if (DuplicateToken(hToken, SecurityImpersonation, &hImpersonatedToken))
				{
					GENERIC_MAPPING mapping = { 0xFFFFFFFF };
					PRIVILEGE_SET privileges = { 0 };
					DWORD grantedAccess = 0, privilegesLength = sizeof(privileges);
					BOOL result = FALSE;

					mapping.GenericRead = FILE_GENERIC_READ;
					mapping.GenericWrite = FILE_GENERIC_WRITE;
					mapping.GenericExecute = FILE_GENERIC_EXECUTE;
					mapping.GenericAll = FILE_ALL_ACCESS;
 
					MapGenericMask(&genericAccessRights, &mapping);
					if (AccessCheck(security, hImpersonatedToken, genericAccessRights, 
						&mapping, &privileges, &privilegesLength, &grantedAccess, &result))
						bRet = (result == TRUE);

					CloseHandle(hImpersonatedToken);
				}

				CloseHandle(hToken);
			}

			free(security);
		}
	}

	return bRet;
}

bool Platform::IsPathReadonly(const path * requestedPath)
{
	return !HasAccessRights(requestedPath->generic_string().c_str(), GENERIC_WRITE);
}
