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

#ifndef _SCREEN_PARTY_OPTIONS_H
#define _SCREEN_PARTY_OPTIONS_H
#pragma once

class ScreenPartyOptions : public Menu
{
public:
	ScreenPartyOptions();
	virtual void OnShow();
	virtual bool ParseInput(Uint32 pressedKey, SDL_Keycode keyCode, bool pressedDown);
	void SetPlaylist2();

protected:
	int _selectLevel;
	int _selectPlaylist;
	int _selectPlaylist2;

	Uint32 _playlistIndex;
	Uint32 _playlist2Index;

	std::vector<std::string> _playlist2Values;
};

#endif
