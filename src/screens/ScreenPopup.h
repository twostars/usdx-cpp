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

#ifndef _SCREEN_POPUP_H
#define _SCREEN_POPUP_H
#pragma once

class ScreenPopup : public Menu
{
public:
	ScreenPopup();
	virtual bool ParseInput(uint32 pressedKey, SDL_Keycode keyCode, bool pressedDown);
	void ShowPopup(const std::string& msg);

	bool Visible;
};

typedef void(*PopupCallback)(bool);
class ScreenPopupCheck : public ScreenPopup
{
public:
	ScreenPopupCheck();
	virtual bool ParseInput(uint32 pressedKey, SDL_Keycode keyCode, bool pressedDown);
	void ShowPopup(const char * message, PopupCallback callback);

protected:
	PopupCallback Callback;
};

class ScreenPopupError : public ScreenPopup
{
public:
	ScreenPopupError();
};

class ScreenPopupInfo : public ScreenPopupError
{
public:
	ScreenPopupInfo();
};

#endif
