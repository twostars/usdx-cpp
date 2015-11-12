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

#ifndef _DISPLAY_H
#define _DISPLAY_H
#pragma once

class Menu;
class AudioPlaybackStream;
class Display : public Singleton<Display>
{
public:
	Display();

	void InitFadeTextures();
	void SaveScreenshot();

	bool Draw();
	void DrawCursor();

	// calls ParseInput of cur or next Screen if assigned
	bool ParseInput(Uint32 pressedKey, SDL_Keycode keyCode, bool pressedDown);
	bool ParseMouse(int mouseButton, bool btnDown, float x, float y);
	bool ParseTextInput(SDL_Event * event);

	// sets SDL_ShowCursor depending on options set in Ini
	void SetCursor();

	// called when cursor moves, positioning of software cursor
	void MoveCursor(float x, float y);

	// called when left or right mouse button is pressed/released
	void OnMouseButton(bool pressed);

	// fades to specific screen (playing specified sound)
	Menu * FadeTo(Menu * screen, const AudioPlaybackStream * sound = NULL);

	// abort fading to the current screen, may be used in OnShow() or during fade process
	void AbortScreenChange();
	
	void DrawDebugInformation();

	// called by MoveCursor and OnMouseButton to update last move and start fade in
	void UpdateCursorFade();

	~Display();

private:
	/*
	HookableEvent ePreDraw;
	HookableEvent eDraw;
	*/

	// fade-to-black
	bool	BlackScreen;
	bool	FadeEnabled;   // true if fading is enabled
	bool	FadeFailed;    // true if fading is not possible (not enough memory, etc.)
	time_t	FadeStartTime; // time when fading starts, 0 meansthat the fade texture must be initialized
	bool	DoneOnShow;    // true if possed onShow after fading
	GLuint	FadeTex[2];
	Uint32	TexW, TexH;

	Uint32	FPSCounter;
	Uint32	LastFPS;
	Uint32	NextFPSSwap;

	std::string	OSD_LastError;

	// software cursor data
	float	CursorX;
	float	CursorY;
	bool	CursorPressed;
	bool	CursorHiddenByScreen; // hides software cursor and deactivates auto fade in

	// used for cursor fade out when there is no movement
	bool	CursorVisible;
	Uint32	CursorLastMove;
	bool	CursorFade;

public:
	Menu *	NextScreen;
	Menu *	CurrentScreen;

	// popup data
	Menu *	NextScreenWithCheck;
	bool	CheckOK;

	// FIXME: Fade is set to 0 in Main & oher files but not used here anymore
	float	Fade;
};

#define sDisplay (Display::getSingleton())

#endif
