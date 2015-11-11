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
#include "ScreenOptionsLyrics.h"

ScreenOptionsLyrics::ScreenOptionsLyrics() : Menu()
{
	ThemeOptionsLyrics * theme = sThemes.OptionsLyrics;

	LoadFromTheme(theme);

	theme->SelectLyricsFont.ShowArrows = true;
	theme->SelectLyricsFont.OneItemOnly = true;
	AddSelectSlide(theme->SelectLyricsFont, (uint32 *)&sIni.LyricsFont, ILyricsFontTranslated, SDL_arraysize(ILyricsFontTranslated));

	theme->SelectLyricsEffect.ShowArrows = true;
	theme->SelectLyricsEffect.OneItemOnly = true;
	AddSelectSlide(theme->SelectLyricsEffect, (uint32 *)&sIni.LyricsEffect, ILyricsEffectTranslated, SDL_arraysize(ILyricsEffectTranslated));

	theme->SelectNoteLines.ShowArrows = true;
	theme->SelectNoteLines.OneItemOnly = true;
	AddSelectSlide(theme->SelectNoteLines, (uint32 *)&sIni.NoteLines, INoteLinesTranslated, SDL_arraysize(INoteLinesTranslated));

	AddButton(theme->ButtonExit);
	if (Buttons[0].Texts.empty())
		AddButtonText(20.0f, 5.0f, sThemes.Options->Description[7]);

	SetInteraction(0);
}

bool ScreenOptionsLyrics::ParseInput(uint32 pressedKey, SDL_Keycode keyCode, bool pressedDown)
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
			if (SelInteraction == 3)
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
			if (SelInteraction >= 0 && SelInteraction <= 3)
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

void ScreenOptionsLyrics::OnShow()
{
	Menu::OnShow();
	SetInteraction(0);
}

void ScreenOptionsLyrics::SaveAndReturn()
{
	sIni.Save();
	sSoundLib.PlaySound(SoundBack);
	FadeTo(UIOptions);
}
