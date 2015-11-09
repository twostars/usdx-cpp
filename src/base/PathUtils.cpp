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

extern CMDParams Params;

path SharedPath;
path UserPath;

path LogPath;
path SongPath;
path ConfigFile;
path ScoreFile;

path SoundPath;
path ThemePath;
path LanguagesPath;
path PluginPath;
path VisualsPath;
path FontPath;
path ResourcesPath;

path PlaylistPath;
path ScreenshotsPath;

PathSet SongPaths;
PathSet CoverPaths;

INLINE std::string GetPath(path& path) {
	return path.generic_string();
}

std::string GetConfigFile() {
	return GetPath(ConfigFile);
}

std::string GetScoreFile() {
	return GetPath(ScoreFile);
}

void AddSongPath(path& path)
{
	if (!path.empty())
		SongPaths.insert(path);
}

void AddCoverPath(path& path)
{
	if (!path.empty())
		CoverPaths.insert(path);
}

bool FindPath(path& resultPath, const path& requestedPath, bool needsWritePermission)
{
	try
	{
		resultPath = requestedPath;

		if (requestedPath.empty()
			|| (!exists(requestedPath) && !create_directory(requestedPath))
			|| (needsWritePermission && Platform::IsPathReadonly(&requestedPath)))
			return false;

		return true;
	}
	catch (boost::filesystem::filesystem_error)
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

		sLog.Warn("InitializePaths", "Log directory (%s) is not available.", 
			LogPath.generic_string().c_str());
	}

	Platform::GetGameSharedPath(&SharedPath);
	Platform::GetGameUserPath(&UserPath);

	FindPath(SoundPath,     SharedPath / SOUND_DIR,    false);
	FindPath(ThemePath,     SharedPath / THEME_DIR,    false);
	FindPath(LanguagesPath, SharedPath / LANG_DIR,     false);
	FindPath(PluginPath,    SharedPath / PLUGIN_DIR,   false);
	FindPath(VisualsPath,   SharedPath / VISUAL_DIR,   false);
	FindPath(FontPath,      SharedPath / FONT_DIR,     false);
	FindPath(ResourcesPath, SharedPath / RESOURCE_DIR, false);

	// Playlists are not shared as we need one directory to write to
	FindPath(PlaylistPath,  UserPath / PLAYLIST_DIR,   false);

	// Screenshot directory (must be writable)
	//if (!FindPath(ScreenshotsPath, UserPath / SCREENSHOT_DIR, true))
	{
		sLog.Warn("InitializePaths", "Screenshot directory (%s) is not available.",
			ScreenshotsPath.generic_string().c_str());
	}

	// Add song paths
	Platform::GetMusicPath(&userMusicDir);

	AddSongPath(SongPath);
	AddSongPath(userMusicDir);
	AddSongPath(UserPath / SONG_DIR);

	// Add category cover paths
	AddCoverPath(SharedPath / COVER_DIR);
	AddCoverPath(UserPath / COVER_DIR);

	// Set config file if not already set by commandline.
	if (ConfigFile.empty())
		ConfigFile = UserPath / CONFIG_FILE;
}
