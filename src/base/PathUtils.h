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

#define VISUAL_DIR		_T("visuals")
#define THEME_DIR		_T("themes")
#define SOUND_DIR		_T("sounds")
#define SONG_DIR		_T("songs")
#define RESOURCE_DIR	_T("resources")
#define PLUGIN_DIR		_T("plugins")
#define LANG_DIR		_T("languages")
#define FONT_DIR		_T("fonts")
#define COVER_DIR		_T("covers")
#define PLAYLIST_DIR    _T("playlists")
#define SCREENSHOT_DIR  _T("screenshots")
#define LOG_DIR			_T("logs")

#define CONFIG_FILE		_T("config.ini")

typedef std::set<boost::filesystem::path> PathSet;

const TCHAR * GetConfigFile();
const TCHAR * GetScoreFile();

void AddSongPath(boost::filesystem::path& path);
void AddCoverPath(boost::filesystem::path& path);

void InitializePaths();

#endif
