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

#ifndef _PLATFORM_H
#define _PLATFORM_H
#pragma once

#include "PathUtils.h"

enum MessageType { mtInfo, mtError };

class Platform
{
public:
	static void Init();

	static bool TerminateIfAlreadyRunning(const char * windowTitle);
	static void ShowMessage(const char * message, MessageType messageType);

	static void DetectLocalExecution();
	static void GetExecutionDir(path * outPath);
	static void GetSpecialPath(int csidl, path * outPath); /* NOTE: Windows only */
	static void GetUserHomeDir(path * outPath);
	static void GetLogPath(path * outPath);
	static void GetGameSharedPath(path * outPath);
	static void GetGameUserPath(path * outPath);
	static void GetMusicPath(path * outPath);

	static bool IsPathReadonly(const path * requestedPath);

	static bool s_useLocalDirs;
};

#endif
