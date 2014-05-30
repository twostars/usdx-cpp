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
#include "../base/Graphic.h"
#include "../base/Language.h"
#include "../base/Ini.h"
#include "../base/Themes.h"
#include "../menu/Menu.h"
#include "ScreenOptions.h"
#include "ScreenOptionsGame.h"
#include "ScreenSong.h"

ScreenOptionsGame::ScreenOptionsGame() : Menu()
{
	ThemeOptionsGame * theme = sThemes.OptionsGame;

	LoadFromTheme(theme);

	theme->SelectPlayers.ShowArrows = true;
	theme->SelectPlayers.OneItemOnly = true;
	AddSelectSlide(theme->SelectPlayers, (uint32 *)&sIni.Players, IPlayers, SDL_arraysize(IPlayers));

	theme->SelectDifficulty.ShowArrows = true;
	theme->SelectDifficulty.OneItemOnly = true;
	AddSelectSlide(theme->SelectDifficulty, (uint32 *)&sIni.Difficulty, IDifficultyTranslated, SDL_arraysize(IDifficultyTranslated));

	LanguageSet& langSet = sLanguage.GetLanguageSet();
	theme->SelectLanguage.ShowArrows = true;
	theme->SelectLanguage.OneItemOnly = true;
	AddSelectSlide(theme->SelectLanguage, (uint32 *)&sIni.Language, &langSet[0], langSet.size());

	theme->SelectTabs.ShowArrows = true;
	theme->SelectTabs.OneItemOnly = true;
	AddSelectSlide(theme->SelectTabs, (uint32 *)&sIni.Tabs, ITabsTranslated, SDL_arraysize(ITabsTranslated));

	theme->SelectSorting.ShowArrows = true;
	theme->SelectSorting.OneItemOnly = true;
	AddSelectSlide(theme->SelectSorting, (uint32 *)&sIni.Sorting, ISortingTranslated, SDL_arraysize(ISortingTranslated));

	theme->SelectDebug.ShowArrows = true;
	theme->SelectDebug.OneItemOnly = true;
	AddSelectSlide(theme->SelectDebug, (uint32 *)&sIni.Debug, IDebugTranslated, SDL_arraysize(IDebugTranslated));

	AddButton(theme->ButtonExit);
	if (Buttons[0].Texts.empty())
		AddButtonText(20.0f, 5.0f, sThemes.Options->Description[7]);

	SetInteraction(0);
}

bool ScreenOptionsGame::ParseInput(uint32 pressedKey, SDL_Keycode keyCode, bool pressedDown)
{
	if (!pressedDown)
		return true;

	switch (pressedKey)
	{
		case SDLK_ESCAPE:
		case SDLK_BACKSPACE:
			SaveAndReturn();
			break;

		case SDLK_RETURN:
			// Back button
			if (SelInteraction == 6)
				SaveAndReturn();
			break;

		case SDLK_DOWN:
			InteractNext();
			break;

		case SDLK_UP:
			InteractPrev();
			break;

		case SDLK_RIGHT:
		case SDLK_LEFT:
			if (SelInteraction >= 0 && SelInteraction <= 5)
			{
				// AudioPlayback.PlaySound(SoundLib.Option);
				if (pressedKey == SDLK_RIGHT)
					InteractInc();
				else
					InteractDec();
			}
			break;
	}

	return true;
}

void ScreenOptionsGame::OnShow()
{
	OldTabs = sIni.Tabs;
	OldSorting = sIni.Sorting;
}

void ScreenOptionsGame::SaveAndReturn()
{
	sIni.Save();

	// AudioPlayback.PlaySound(SoundLib.Back); // TODO
	RefreshSongs();
	FadeTo(UIOptions);
}

void ScreenOptionsGame::RefreshSongs()
{
	if (sIni.Sorting != OldSorting
		|| sIni.Tabs != OldTabs)
		UISong->Refresh();
}
