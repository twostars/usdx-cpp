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
#include "../base/Ini.h"
#include "../base/Themes.h"
#include "../base/Graphic.h"
#include "../base/Music.h"
#include "../menu/Menu.h"
#include "ScreenLevel.h"
#include "ScreenMain.h"
#include "ScreenName.h"
#include "ScreenSong.h"

ScreenLevel::ScreenLevel() : Menu()
{
	const ThemeLevel * theme = sThemes.Level;

	LoadFromTheme(theme);

	AddButton(theme->ButtonEasy);
	AddButton(theme->ButtonMedium);
	AddButton(theme->ButtonHard);

	SetInteraction(0);
}

void ScreenLevel::OnShow()
{
	Menu::OnShow();

	// Set the selected difficulty level.
	SetInteraction(sIni.Difficulty);
}

bool ScreenLevel::ParseInput(Uint32 pressedKey, SDL_Keycode keyCode, bool pressedDown)
{
	if (!pressedDown)
		return true;
	
	switch (pressedKey)
	{
		case SDLK_ESCAPE:
		case SDLK_BACKSPACE:
			if (sIni.OnSongClick == eSongClickType::SelectPlayers)
				FadeTo(UIMain, SoundBack);
			else
				FadeTo(UIName, SoundBack);
			break;

		case SDLK_RETURN:
			sIni.Difficulty = (eDifficultyType) SelInteraction;
			sIni.SaveGameSettings();
			sIni.SaveToFile();

			// Set standard mode
			// UISong->Mode = smNormal;
			FadeTo(UISong, SoundStart);
			break;

		case SDLK_DOWN:
		case SDLK_RIGHT:
			InteractNext();
			break;

		case SDLK_UP:
		case SDLK_LEFT:
			InteractPrev();
			break;
	}

	return true;
}

void ScreenLevel::SetAnimationProgress(float progress)
{
	Buttons[0].Tex.ScaleW = progress;
	Buttons[1].Tex.ScaleW = progress;
	Buttons[2].Tex.ScaleW = progress;
}
