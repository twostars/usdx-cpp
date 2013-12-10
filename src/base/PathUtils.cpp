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
#include "Log.h"
#include "CommandLine.h"

#include <boost/filesystem.hpp>

using namespace boost::filesystem;

extern CMDParams Params;

path SharedPath;
path UserPath;

path LogPath;
path SongPath;
path ConfigFile;
path ScoreFile;

path SoundPath;
path ThemePath;
path SkinsPath;
path LanguagesPath;
path PluginPath;
path VisualsPath;
path FontPath;
path ResourcesPath;

path PlaylistPath;
path ScreenshotsPath;

PathSet SongPaths;
PathSet CoverPaths;

bool FindPath(path& resultPath, const path& requestedPath, bool needsWritePermission)
{
	try
	{
		if (requestedPath.empty()
			|| (!exists(requestedPath) 
				&& !create_directory(requestedPath)))
			return false;

		resultPath = requestedPath;

		if (needsWritePermission
			&& Platform::IsPathReadonly(&requestedPath))
			return false;

		return true;
	}
	catch (filesystem_error)
	{
		return false;
	}
}

void InitializePaths()
{
	Platform::Init();
	path logPath, userMusicDir;

	Platform::GetLogPath(&logPath);
	if (!FindPath(LogPath, logPath, true))
	{
		Params.NoLog = true;

		// TODO: Provide path.
		sLog.Warn(_T("InitializePaths"), _T("Log directory is not available."));
	}

	Platform::GetGameSharedPath(&SharedPath);
	Platform::GetGameUserPath(&UserPath);

	FindPath(SoundPath,     SharedPath / SOUND_DIR,    false);
	FindPath(ThemePath,     SharedPath / THEME_DIR,    false);
	FindPath(SkinsPath,     SharedPath / SKINS_DIR,    false);
	FindPath(LanguagesPath, SharedPath / LANG_DIR,     false);
	FindPath(PluginPath,    SharedPath / PLUGIN_DIR,   false);
	FindPath(VisualsPath,   SharedPath / VISUAL_DIR,   false);
	FindPath(FontPath,      SharedPath / FONT_DIR,     false);
	FindPath(ResourcesPath, SharedPath / RESOURCE_DIR, false);

	// Playlists are not shared as we need one directory to write to
	FindPath(PlaylistPath,  UserPath / PLAYLIST_DIR,   false);

	// Screenshot directory (must be writable)
	if (!FindPath(ScreenshotsPath, UserPath / SCREENSHOT_DIR, true))
	{
		// TODO: Provide path.
		sLog.Warn(_T("InitializePaths"), _T("Screenshot directory is not available."));
	}

	// Add song paths
	Platform::GetMusicPath(&userMusicDir);

	SongPaths.insert(SongPath);
	SongPaths.insert(userMusicDir);
	SongPaths.insert(UserPath / SONG_DIR);

	// Add category cover paths
	CoverPaths.insert(SharedPath / COVER_DIR);
	CoverPaths.insert(UserPath / COVER_DIR);
}
