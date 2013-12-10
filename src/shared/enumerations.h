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

#ifndef _ENUMERATIONS_H
#define _ENUMERATIONS_H
#pragma once

#define IMPROVED_ENUM_NAME  Color
#define IMPROVED_ENUM_LIST  ENUMITEM(Blue)   \
							ENUMITEM(Green)  \
							ENUMITEM(Pink)   \
							ENUMITEM(Red)    \
							ENUMITEM(Violet) \
							ENUMITEM(Orange) \
							ENUMITEM(Yellow) \
							ENUMITEM(Brown)  \
							ENUMITEM(Black)  \
							ENUMITEM(Total)
#include <DefineImprovedEnum.h>

#define IMPROVED_ENUM_NAME  DifficultyType
#define IMPROVED_ENUM_LIST  ENUMITEM(Easy)   \
							ENUMITEM(Medium) \
							ENUMITEM(Hard)
#include <DefineImprovedEnum.h>


#define IMPROVED_ENUM_NAME  SortingType
#define IMPROVED_ENUM_LIST  ENUMITEM(Edition)   \
							ENUMITEM(Genre)     \
							ENUMITEM(Language)  \
							ENUMITEM(Folder)    \
							ENUMITEM(Title)     \
							ENUMITEM(Artist)    \
							ENUMITEM(Artist2)   \
							ENUMITEM(Total)
#include <DefineImprovedEnum.h>

#define IMPROVED_ENUM_NAME  SingWindowType
#define IMPROVED_ENUM_LIST  ENUMITEM(Small)   \
							ENUMITEM(Big)
#include <DefineImprovedEnum.h>

#define IMPROVED_ENUM_NAME  Switch
#define IMPROVED_ENUM_LIST  ENUMITEM(Off)   \
							ENUMITEM(On)
#include <DefineImprovedEnum.h>

#define IMPROVED_ENUM_NAME  LyricsFontType
#define IMPROVED_ENUM_LIST  ENUMITEM(Plain)   \
							ENUMITEM(OLine1)  \
							ENUMITEM(OLine2)
#include <DefineImprovedEnum.h>

#define IMPROVED_ENUM_NAME  LyricsEffectType
#define IMPROVED_ENUM_LIST  ENUMITEM(Simple)   \
							ENUMITEM(Zoom)     \
							ENUMITEM(Slide)    \
							ENUMITEM(Ball)     \
							ENUMITEM(Shift)
#include <DefineImprovedEnum.h>

#define IMPROVED_ENUM_NAME  VisualizerOption
#define IMPROVED_ENUM_LIST  ENUMITEM(Off)          \
							ENUMITEM(WhenNoVideo)  \
							ENUMITEM(On)
#include <DefineImprovedEnum.h>

#define IMPROVED_ENUM_NAME  SongClickType
#define IMPROVED_ENUM_LIST  ENUMITEM(Sing)          \
							ENUMITEM(SelectPlayers) \
							ENUMITEM(OpenMenu)
#include <DefineImprovedEnum.h>

#define IMPROVED_ENUM_NAME  SyncToType
#define IMPROVED_ENUM_LIST  ENUMITEM(Music)  \
							ENUMITEM(Lyrics) \
							ENUMITEM(Off)
#include <DefineImprovedEnum.h>

#endif
