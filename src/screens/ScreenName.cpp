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
#include "ScreenSing.h"
#include "ScreenSong.h"

ScreenName::ScreenName() : Menu(), GotoSingScreen(false)
{
	const ThemeName * theme = sThemes.Name;

	LoadFromTheme(theme);
	for (int player = 0; player < MAX_PLAYERS; player++)
		AddButton(theme->ButtonPlayer[player]);

	SetInteraction(0);
}

void ScreenName::OnShow()
{
	Menu::OnShow();

	for (int player = 0; player < MAX_PLAYERS; player++)
		Buttons[player].Texts[0].SetText(sIni.Name[player]);

	int players = sIni.Players + 1;
	for (int player = 0; player < players; player++)
	{
		Buttons[player].Visible = true;
		Buttons[player].Selectable = true;
	}

	for (int player = players; player < MAX_PLAYERS; player++)
	{
		Buttons[player].Visible = false;
		Buttons[player].Selectable = false;
	}
}

bool ScreenName::ParseInput(Uint32 pressedKey, SDL_Keycode keyCode, bool pressedDown)
{
	if (!pressedDown)
		return true;

	Uint16 modState = SDL_GetModState()
		& (KMOD_SHIFT | KMOD_CTRL | KMOD_ALT);

	MenuButton& selectedButton = Buttons[SelInteraction];
	if (pressedKey >= SDLK_F1 && pressedKey <= SDLK_F12)
	{
		size_t index = pressedKey - SDLK_F1;
		std::string buttonText = selectedButton.Texts[0].GetText();
		if (modState & KMOD_ALT)
			sIni.NameTemplate[index] = buttonText;
		else
			buttonText = sIni.NameTemplate[index];
	}
	else
	{
		switch (pressedKey)
		{
			case SDLK_BACKSPACE:
				selectedButton.Texts[0].DeleteLastLetter();
				break;

			case SDLK_ESCAPE:
				sIni.SaveProfileSettings();
				sIni.SaveToFile();
				sSoundLib.PlaySound(SoundBack);

				if (GotoSingScreen)
					FadeTo(UISong);
				else
					FadeTo(UIMain);
				break;

			case SDLK_RETURN:
				for (int player = 0; player < MAX_PLAYERS; player++)
					sIni.Name[player] = Buttons[player].Texts[0].GetText();

				sIni.SaveProfileSettings();
				sIni.SaveToFile();
				sSoundLib.PlaySound(SoundStart);

				if (GotoSingScreen)
					FadeTo(UISing);
				else
					FadeTo(UILevel);

				GotoSingScreen = false;
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
	}

	return true;
}

bool ScreenName::ParseTextInput(SDL_Event * event)
{
	// TODO: Append name text to the selected button
	// This needs to be handled differently because of SDL2
	// if (IsPrintableChar(charCode))
	// {
	//	Buttons[SelInteraction].Texts[0].Text += (std::string) charCode;
	//	return true;
	// }

	if (event->type == SDL_TEXTINPUT)
	{
		auto& menuText = Buttons[SelInteraction].Texts[0];
		menuText.SetText(menuText.GetText() + event->text.text);
	}

	return true;
}


void ScreenName::SetAnimationProgress(float progress)
{
	for (int player = 0; player < MAX_PLAYERS; player++)
		Buttons[player].DeselectTexture.ScaleW = progress;
}
