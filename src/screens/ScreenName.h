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

#ifndef _SCREEN_NAME_H
#define _SCREEN_NAME_H
#pragma once

class ScreenName : public Menu
{
public:
	ScreenName();

	virtual void OnShow();
	virtual bool ParseInput(uint32 pressedKey, SDL_Keycode keyCode, bool pressedDown);
	virtual bool ParseTextInput(Uint32 inputType, SDL_Event * event);
	void SetAnimationProgress(float progress);

	// If true then next screen in sing screen
	bool GotoSingScreen;
};

#endif
