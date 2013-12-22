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

#ifndef _SCREENS_H
#define _SCREENS_H
#pragma once

#include "../menu/Menu.h"
#include "../screens/ScreenLoading.h"
#include "../screens/ScreenMain.h"
#include "../screens/ScreenName.h"
#include "../screens/ScreenLevel.h"
#include "../screens/ScreenSong.h"
#include "../screens/ScreenSing.h"
#include "../screens/ScreenScore.h"
#include "../screens/ScreenTop5.h"
#include "../screens/ScreenOptions.h"
#include "../screens/ScreenOptionsGame.h"
#include "../screens/ScreenOptionsGraphics.h"
#include "../screens/ScreenOptionsSound.h"
#include "../screens/ScreenOptionsLyrics.h"
#include "../screens/ScreenOptionsThemes.h"
#include "../screens/ScreenOptionsRecord.h"
#include "../screens/ScreenOptionsAdvanced.h"
#include "../screens/ScreenEditSub.h"
#include "../screens/ScreenEdit.h"
#include "../screens/ScreenEditConvert.h"
#include "../screens/ScreenEditHeader.h"
#include "../screens/ScreenOpen.h"
#include "../screens/ScreenSongMenu.h"
#include "../screens/ScreenSongJumpTo.h"
#include "../screens/ScreenPartyNewRound.h"
#include "../screens/ScreenPartyScore.h"
#include "../screens/ScreenPartyWin.h"
#include "../screens/ScreenPartyOptions.h"
#include "../screens/ScreenPartyPlayer.h"
#include "../screens/ScreenPartyRounds.h"
#include "../screens/ScreenStatMain.h"
#include "../screens/ScreenStatDetail.h"
#include "../screens/ScreenCredits.h"
#include "../screens/ScreenPopup.h"

// Screens
extern ScreenLoading			* UILoading;
extern ScreenMain				* UIMain;
extern ScreenName				* UIName;
extern ScreenLevel				* UILevel;
extern ScreenSong				* UISong;
extern ScreenSing				* UISing;
extern ScreenScore				* UIScore;
extern ScreenTop5				* UITop5;
extern ScreenOptions			* UIOptions;
extern ScreenOptionsGame		* UIOptionsGame;
extern ScreenOptionsGraphics	* UIOptionsGraphics;
extern ScreenOptionsSound		* UIOptionsSound;
extern ScreenOptionsLyrics		* UIOptionsLyrics;
extern ScreenOptionsThemes		* UIOptionsThemes;
extern ScreenOptionsRecord		* UIOptionsRecord;
extern ScreenOptionsAdvanced	* UIOptionsAdvanced;
extern ScreenEditSub			* UIEditSub;
extern ScreenEdit				* UIEdit;
extern ScreenEditConvert		* UIEditConvert;
extern ScreenEditHeader			* UIEditHeader;
extern ScreenOpen				* UIOpen;

extern ScreenSongMenu			* UISongMenu;
extern ScreenSongJumpTo			* UISongJumpTo;

// Party screens
extern ScreenPartyNewRound		* UIPartyNewRound;
extern ScreenPartyScore			* UIPartyScore;
extern ScreenPartyWin			* UIPartyWin;
extern ScreenPartyOptions		* UIPartyOptions;
extern ScreenPartyPlayer		* UIPartyPlayer;
extern ScreenPartyRounds		* UIPartyRounds;

// Stats screens
extern ScreenStatMain			* UIStatMain;
extern ScreenStatDetail			* UIStatDetail;

// Credits screen
extern ScreenCredits			* UICredits;

// Popups
extern ScreenPopupCheck			* UIPopupCheck;
extern ScreenPopupError			* UIPopupError;
extern ScreenPopupInfo			* UIPopupInfo;

#endif
