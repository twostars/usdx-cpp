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
#include "../base/Language.h"
#include "../base/Themes.h"
#include "../menu/Menu.h"
#include "ScreenPopup.h"

ScreenPopup::ScreenPopup()
	: Menu(), Visible(false)
{
}

ScreenPopupCheck::ScreenPopupCheck()
	: ScreenPopup(), Callback(NULL)
{
	const ThemeCheckPopup * theme = sThemes.CheckPopup;

	AddText(theme->TextCheck);

	LoadFromTheme(theme);

	AddButton(theme->Button1);
	if (Buttons[0].Texts.empty())
		AddButtonText(14.0f, 20.0f, _T("Button 1"));

	AddButton(theme->Button2);
	if (Buttons[1].Texts.empty())
		AddButtonText(14.0f, 20.0f, _T("Button 2"));

	SetInteraction(0);
}

void ScreenPopupCheck::ShowPopup(const TCHAR * message, PopupCallback callback)
{
	Visible = true;
	Callback = callback;

	Texts[0].TextString = sLanguage.Translate(message);

	MenuButton& yesButton = Buttons[0];
	MenuButton& noButton = Buttons[1];

	yesButton.Visible = true;
	yesButton.Texts[0].TextString = sLanguage.Translate(_T("SONG_MENU_YES"));

	noButton.Visible = true;
	noButton.Texts[0].TextString = sLanguage.Translate(_T("SONG_MENU_NO"));

	Background->OnShow();
}
