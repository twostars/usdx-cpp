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
#include "../base/Themes.h"
#include "../base/Music.h"
#include "../menu/Menu.h"
#include "ScreenMain.h"
#include "ScreenOptions.h"
#include "ScreenOptionsGame.h"
#include "ScreenOptionsGraphics.h"
#include "ScreenOptionsSound.h"
#include "ScreenOptionsSound.h"
#include "ScreenOptionsLyrics.h"
#include "ScreenOptionsThemes.h"
#include "ScreenOptionsRecord.h"
#include "ScreenOptionsAdvanced.h"

ScreenOptions::ScreenOptions() : Menu()
{
	const ThemeOptions * theme = sThemes.Options;

	TextDescription = AddText(theme->TextDescription);
	LoadFromTheme(theme);

	AddButton(theme->ButtonGame);
	if (Buttons[0].Texts.empty())
		AddButtonText(14.0f, 20.0f, theme->Description[0]);

	AddButton(theme->ButtonGraphics);
	if (Buttons[1].Texts.empty())
		AddButtonText(14.0f, 20.0f, theme->Description[1]);

	AddButton(theme->ButtonSound);
	if (Buttons[2].Texts.empty())
		AddButtonText(14.0f, 20.0f, theme->Description[2]);

	AddButton(theme->ButtonLyrics);
	if (Buttons[3].Texts.empty())
		AddButtonText(14.0f, 20.0f, theme->Description[3]);

	AddButton(theme->ButtonThemes);
	if (Buttons[4].Texts.empty())
		AddButtonText(14.0f, 20.0f, theme->Description[4]);

	AddButton(theme->ButtonRecord);
	if (Buttons[5].Texts.empty())
		AddButtonText(14.0f, 20.0f, theme->Description[5]);

	AddButton(theme->ButtonAdvanced);
	if (Buttons[6].Texts.empty())
		AddButtonText(14.0f, 20.0f, theme->Description[6]);

	AddButton(theme->ButtonExit);
	if (Buttons[7].Texts.empty())
		AddButtonText(14.0f, 20.0f, theme->Description[7]);

	SetInteraction(0);
}

bool ScreenOptions::ParseInput(Uint32 pressedKey, SDL_Keycode keyCode, bool pressedDown)
{
	if (!pressedDown)
		return true;

	switch (pressedKey)
	{
		case SDLK_ESCAPE:
		case SDLK_BACKSPACE:
			sIni.Save();
			sSoundLib.PlaySound(SoundBack);
			FadeTo(UIMain);
			break;

		case SDLK_RETURN:
			// Back
			if (SelInteraction == 7)
				return ParseInput(SDLK_ESCAPE, SDLK_ESCAPE, pressedDown);

			sSoundLib.PlaySound(SoundStart);

			switch (SelInteraction)
			{
				case 0:
					FadeTo(UIOptionsGame);
					break;

				case 1:
					FadeTo(UIOptionsGraphics);
					break;

				case 2:
					FadeTo(UIOptionsSound);
					break;

				case 3:
					FadeTo(UIOptionsLyrics);
					break;

				case 4:
					FadeTo(UIOptionsThemes);
					break;

				case 5:
					FadeTo(UIOptionsRecord);
					break;

				case 6:
					FadeTo(UIOptionsAdvanced);
					break;
			}
			break;

		case SDLK_DOWN:
			InteractNextRow();
			break;

		case SDLK_UP:
			InteractPrevRow();
			break;

		case SDLK_RIGHT:
			InteractNext();
			break;

		case SDLK_LEFT:
			InteractPrev();
			break;
	}

	return true;
}

void ScreenOptions::SetAnimationProgress(float progress)
{
	for (int i = 0; i < 8; i++)
		Buttons[i].Tex.ScaleW = progress;
}

void ScreenOptions::OnShow()
{
	Menu::OnShow();

	// continue possibly stopped bg-music (stopped in record options)
	sSoundLib.StartBgMusic();
}

void ScreenOptions::OnInteraction()
{
	Texts[TextDescription].SetText(sThemes.Options->Description[SelInteraction]);
}
