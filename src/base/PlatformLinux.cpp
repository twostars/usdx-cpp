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
#include "Platform.h"

#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

#include <boost/filesystem.hpp>

using namespace boost::filesystem;

bool Platform::TerminateIfAlreadyRunning(const char * windowTitle)
{
	return false;
}

void Platform::ShowMessage(const char * message, MessageType messageType)
{
	printf("%s\n", message);
}

/**
 * Detects whether the game was executed locally or globally.
 * - It is local if it was not installed and directly executed from
 *   within the game folder. In this case resources (themes, language-files)
 *   reside in the directory of the executable.
 * - It is global if the game was installed (e.g. to /usr/bin) and
 *   the resources are in a separate folder (e.g. /usr/share/ultrastardx)
 *   which name is stored in the INSTALL_DATADIR constant in paths.inc.
 *
 * Sets UseLocalDirs to true if the game is executed locally, false otherwise.
 */
void Platform::DetectLocalExecution()
{
	path langDir(current_path());
	langDir /= LANG_DIR;
	s_useLocalDirs = exists(langDir);
}

void Platform::GetUserHomeDir(path * outPath)
{
	char * homeDir = getenv("HOME");
	if (homeDir != NULL)
	{
		*outPath = homeDir;
		return;
    }
 
	uid_t uid = getuid();
	struct passwd *pw = getpwuid(uid);
 
	if (pw == NULL)
		throw CriticalException("Failed to identify user's home directory.");

	*outPath = pw->pw_dir;
}

void Platform::GetLogPath(path * outPath)
{
	if (s_useLocalDirs)
		return GetExecutionDir(outPath);

	GetGameUserPath(outPath);
	*outPath /= LOG_DIR;
}

void Platform::GetGameSharedPath(path * outPath)
{
	if (s_useLocalDirs)
		return GetExecutionDir(outPath);

	*outPath = INSTALL_DATADIR;
}

void Platform::GetGameUserPath(path * outPath)
{
	if (s_useLocalDirs)
		return GetExecutionDir(outPath);

	GetUserHomeDir(outPath);
	*outPath /= "." USDX_IDENTIFIER;
}

void Platform::GetMusicPath(path * outPath)
{
	GetGameSharedPath(outPath);
}

bool Platform::IsPathReadonly(const path * requestedPath)
{
	file_status s = status(*requestedPath);
	perms p = s.permissions();
	return (p & others_write) != others_write;
}
