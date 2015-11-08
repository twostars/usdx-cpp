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

#ifndef _PATHUTILS_H
#define _PATHUTILS_H
#pragma once

#define VISUAL_DIR		"visuals"
#define THEME_DIR		"themes"
#define SOUND_DIR		"sounds"
#define SONG_DIR		"songs"
#define RESOURCE_DIR	"resources"
#define PLUGIN_DIR		"plugins"
#define LANG_DIR		"languages"
#define FONT_DIR		"fonts"
#define COVER_DIR		"covers"
#define PLAYLIST_DIR    "playlists"
#define SCREENSHOT_DIR  "screenshots"
#define LOG_DIR			"logs"
#define ICONS_DIR		"icons"

#define CONFIG_FILE		"config.ini"
#define FONTS_FILE		"fonts.ini"

typedef std::set<path> PathSet;

std::string GetConfigFile();
std::string GetScoreFile();

void AddSongPath(path& path);
void AddCoverPath(path& path);

void InitializePaths();

extern path SharedPath;
extern path UserPath;

extern path LogPath;
extern path SongPath;
extern path ConfigFile;
extern path ScoreFile;

extern path SoundPath;
extern path ThemePath;
extern path LanguagesPath;
extern path PluginPath;
extern path VisualsPath;
extern path FontPath;
extern path ResourcesPath;

extern path PlaylistPath;
extern path ScreenshotsPath;

#endif
