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
#include "../base/Themes.h"
#include "../base/Language.h"
#include "../menu/Menu.h"
#include "../screens/ScreenEdit.h"
#include "../screens/ScreenPopup.h"
#include "../screens/ScreenLevel.h"
#include "../screens/ScreenPartyOptions.h"
#include "../screens/ScreenOptions.h"
#include "../screens/ScreenStatMain.h"
#include "ScreenMain.h"

ScreenMain::ScreenMain() : Menu(), _userInteractionTicks(0)
{
	const ThemeMain* theme = sThemes.Main;

	// Creation order needed because of LoadFromTheme() 
	// & button collections.
	// First we must setup custom texts & statics, then LoadFromTheme,
	// and finally the buttons & select sliders.
	TextDescription     = AddText(theme->TextDescription);
	TextDescriptionLong = AddText(theme->TextDescriptionLong);

	LoadFromTheme(theme);

	AddButton(theme->ButtonSolo);
	AddButton(theme->ButtonMulti);
	AddButton(theme->ButtonStats);
	AddButton(theme->ButtonEditor);
	AddButton(theme->ButtonOptions);
	AddButton(theme->ButtonExit);

	SetInteraction(0);
}

bool ScreenMain::ParseInput(uint32 pressedKey, SDL_Keycode keyCode, bool pressedDown)
{
	_userInteractionTicks = SDL_GetTicks();

	if (pressedDown)
	{
		// TODO: Special key combinations 
		// Q->quit, ALT+C->Credits, M->party options/"modes", S->Stats, E->Edit

		switch (pressedKey)
		{
			case SDLK_ESCAPE:
			case SDLK_BACKSPACE:
				return false;

			case SDLK_RETURN:
				switch (SelInteraction)
				{
					// Solo button
					case 0:
						// if (Songs.SongList.Count >= 1)
						// {

						//		if (sIni.Players >= 0) && (sIni.Players <= 3)
						//			PlayersPlay = Ini.Players + 1;

						//		if (sIni.Players == 4)
						//			PlayersPlay = 6;

						//		if (sIni.OnSongClick == sSelectPlayer)
						FadeTo(UILevel);
						//		else
						//		{
						//			UIName->Goto_SingScreen = false;
						//			FadeTo(UIName, /*SoundLib.Start*/ NULL); // TODO
						//		}
						//	}
						//	else // show error message
						//		UIPopupError->ShowPopup(sLanguage.Translate(_T("ERROR_NO_SONGS")));
						break;

					// Multi
					case 1:
						// if (Songs.SongList.Count >= 1)
						FadeTo(UIPartyOptions, /*SoundLib.Start*/ NULL); // TODO
						break;

					// Stats
					case 2:
						FadeTo(UIStatMain, /*SoundLib.Start*/ NULL); // TODO
						break;

					// Editor:
					case 3:
		//#if defined(UseMIDIPort)
						FadeTo(UIEdit, /*SoundLib.Start*/ NULL); // TODO
		//#else
		//				UIPopupError->ShowPopup(sLanguage.Translate(_T("ERROR_NO_EDITOR")));
		//#endif
						break;

					// Options
					case 4:
						FadeTo(UIOptions, /*SoundLib.Start*/ NULL); // TODO
						break;

					// Exit
					case 5:
						return false;
						}
				break;
		}
	}

	return true;
}
