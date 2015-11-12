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
#include "../base/Music.h"
#include "../menu/Menu.h"
#include "ScreenOptions.h"
#include "ScreenOptionsAdvanced.h"

ScreenOptionsAdvanced::ScreenOptionsAdvanced() : Menu()
{
	ThemeOptionsAdvanced * theme = sThemes.OptionsAdvanced;

	LoadFromTheme(theme);
	// SelectLoadAnimation hidden because it is useless atm
	// AddSelect(theme->SelectLoadAnimation, sIni.LoadAnimation, ILoadAnimationTranslated);

	theme->SelectScreenFade.ShowArrows = true;
	theme->SelectScreenFade.OneItemOnly = true;
	AddSelectSlide(theme->SelectScreenFade, (Uint32 *)&sIni.ScreenFade, IScreenFadeTranslated, SDL_arraysize(IScreenFadeTranslated));

	theme->SelectEffectSing.ShowArrows = true;
	theme->SelectEffectSing.OneItemOnly = true;
	AddSelectSlide(theme->SelectEffectSing, (Uint32 *)&sIni.EffectSing, IEffectSingTranslated, SDL_arraysize(IEffectSingTranslated));

	theme->SelectLineBonus.ShowArrows = true;
	theme->SelectLineBonus.OneItemOnly = true;
	AddSelectSlide(theme->SelectLineBonus, (Uint32 *)&sIni.LineBonus, ILineBonusTranslated, SDL_arraysize(ILineBonusTranslated));

	theme->SelectOnSongClick.ShowArrows = true;
	theme->SelectOnSongClick.OneItemOnly = true;
	AddSelectSlide(theme->SelectOnSongClick, (Uint32 *)&sIni.OnSongClick, IOnSongClickTranslated, SDL_arraysize(IOnSongClickTranslated));

	theme->SelectAskBeforeDel.ShowArrows = true;
	theme->SelectAskBeforeDel.OneItemOnly = true;
	AddSelectSlide(theme->SelectAskBeforeDel, (Uint32 *)&sIni.AskBeforeDel, IAskBeforeDelTranslated, SDL_arraysize(IAskBeforeDelTranslated));

	theme->SelectPartyPopup.ShowArrows = true;
	theme->SelectPartyPopup.OneItemOnly = true;
	AddSelectSlide(theme->SelectPartyPopup, (Uint32 *)&sIni.PartyPopup, IPartyPopupTranslated, SDL_arraysize(IPartyPopupTranslated));

	AddButton(theme->ButtonExit);
	if (Buttons[0].Texts.empty())
		AddButtonText(20.0f, 5.0f, sThemes.Options->Description[7]);

	SetInteraction(0);
}

bool ScreenOptionsAdvanced::ParseInput(Uint32 pressedKey, SDL_Keycode keyCode, bool pressedDown)
{
	if (!pressedDown)
		return true;

	switch (pressedKey)
	{
		case SDLK_ESCAPE:
		case SDLK_BACKSPACE:
			sIni.Save();
			sSoundLib.PlaySound(SoundBack);
			FadeTo(UIOptions);
			break;

		case SDLK_RETURN:
			if (SelInteraction == 6)
			{
				sIni.Save();
				sSoundLib.PlaySound(SoundBack);
				FadeTo(UIOptions);
			}
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
				sSoundLib.PlaySound(SoundOption);

				if (pressedKey == SDLK_RIGHT)
					InteractInc();
				else
					InteractDec();
			}
			break;
	}

	return true;
}

void ScreenOptionsAdvanced::OnShow()
{
	Menu::OnShow();
	SetInteraction(0);
}
