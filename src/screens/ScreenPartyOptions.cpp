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
#include "../base/Ini.h"
#include "../base/Language.h"
#include "../base/Music.h"
#include "../menu/Menu.h"
#include "../menu/Display.h"
#include "../base/Themes.h"
#include "ScreenPartyOptions.h"
#include "ScreenMain.h"
#include "ScreenPopup.h"
#include "ScreenPartyPlayer.h"

static std::string IPlaylistTranslated[3];

ScreenPartyOptions::ScreenPartyOptions() : Menu()
{
	ThemePartyOptions * theme = sThemes.PartyOptions;

	// Translate playlist values
	IPlaylistTranslated[0] = __T("PARTY_PLAYLIST_ALL");
	IPlaylistTranslated[1] = __T("PARTY_PLAYLIST_CATEGORY");
	IPlaylistTranslated[2] = __T("PARTY_PLAYLIST_PLAYLIST");

	// Clear all sliders
	_playlistIndex = 0;
	_playlist2Index = 0;
	_playlist2Values.push_back("---");

	LoadFromTheme(theme);

	theme->SelectLevel.OneItemOnly = true;
	theme->SelectLevel.ShowArrows = true;
	_selectLevel = AddSelectSlide(theme->SelectLevel, (Uint32 *)&sIni.Difficulty, IDifficultyTranslated, SDL_arraysize(IDifficultyTranslated));

	theme->SelectPlaylist.OneItemOnly = true;
	theme->SelectPlaylist.ShowArrows = true;
	_selectPlaylist = AddSelectSlide(theme->SelectPlaylist, &_playlistIndex, IPlaylistTranslated, SDL_arraysize(IPlaylistTranslated));

	theme->SelectPlaylist2.OneItemOnly = true;
	theme->SelectPlaylist2.ShowArrows = true;
	_selectPlaylist2 = AddSelectSlide(theme->SelectPlaylist2, &_playlist2Index, &_playlist2Values[0], _playlist2Values.size());

	SetInteraction(0);
}

void ScreenPartyOptions::OnShow()
{
	Menu::OnShow();

	/*
	Party.Clear();

	// check if there are loaded modes
	if (Party.ModesAvailable)
	{
		// modes are loaded
		Randomize();
	}
	else
	{
		// no modes found
		UIPopupError->ShowPopup(__T("ERROR_NO_PLUGINS"));
		sDisplay.AbortScreenChange();
	}
	*/
}

bool ScreenPartyOptions::ParseInput(Uint32 pressedKey, SDL_Keycode keyCode, bool pressedDown)
{
	if (pressedDown)
	{
		// TODO: Special key combinations 
		// Q->quit

		switch (pressedKey)
		{
		case SDLK_ESCAPE:
		case SDLK_BACKSPACE:
			FadeTo(UIMain, SoundBack);
			break;

		case SDLK_RETURN:
			// TODO: Don't start when playlist is selected and there are no playlists
			// if (_playlistIndex == 2 && playlistCount == 0)
			//	return;

			// Save the difficulty
			sIni.SaveGameSettings();

			// TODO: Save the playlist etc

			// Go to the player screen
			FadeTo(UIPartyPlayer, SoundStart);
			break;

		case SDLK_DOWN:
			InteractNext();
			break;

		case SDLK_UP:
			InteractPrev();
			break;

		case SDLK_RIGHT:
			sSoundLib.PlaySound(SoundOption);
			InteractInc();

			// Change playlist2 if playlist is changed
			if (SelInteraction == 1)
				SetPlaylist2();
			break;

		case SDLK_LEFT:
			sSoundLib.PlaySound(SoundOption);
			InteractDec();

			// Change playlist2 if playlist is changed
			if (SelInteraction == 1)
				SetPlaylist2();
			break;
		}
	}

	return true;
}

void ScreenPartyOptions::SetPlaylist2()
{
	MenuSelectSlide& playlist2 = SelectSlides[_selectPlaylist2];

	switch (_playlistIndex)
	{
	// All songs
	case 0:
		ResetSelectSlideOptions(_selectPlaylist2);
		_playlist2Values.clear();
		_playlist2Values.push_back("---");
		UpdateSelectSlideOptions(_selectPlaylist2, _playlist2Values, 0);
		break;

	// Folder
	case 1:
		ResetSelectSlideOptions(_selectPlaylist2);
		_playlist2Values.clear();

		// TODO: Build list

		_playlist2Values.push_back("No Categories found"); // TODO: translate
		UpdateSelectSlideOptions(_selectPlaylist2, _playlist2Values, 0);
		break;

	// Playlist
	case 2:
		ResetSelectSlideOptions(_selectPlaylist2);
		_playlist2Values.clear();

		// TODO: Build list

		_playlist2Values.push_back("No Playlists found"); // TODO: translate
		UpdateSelectSlideOptions(_selectPlaylist2, _playlist2Values, 0);
		break;
	}
}
