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
#include "Display.h"

#include "../base/Log.h"
#include "../base/Time.h"
#include "../base/Ini.h"
#include "../base/CommandLine.h"
#include "../base/Graphic.h"
#include "../base/TextGL.h"

#include "Menu.h"

#include "../screens/ScreenSing.h"
#include "../screens/ScreenPopup.h"

initialiseSingleton(Display);

extern CMDParams Params;

// constants for screen transition
// time in milliseconds
static const float FADE_DURATION = 400.0f;

// constants for software cursor effects
// time in milliseconds
static const uint32 CURSOR_FADE_IN_TIME = 500;      // seconds the fade in effect lasts
static const uint32 CURSOR_FADE_OUT_TIME = 2000;    // seconds the fade out effect lasts
static const uint32 CURSOR_AUTOHIDE_TIME = 5000;   // seconds until auto fade out starts if there is no mouse movement

Display::Display()
// create events for plugins
//	: ePreDraw("Display.PreDraw"), eDraw("Display.Draw")
{
	// init popup
	CheckOK             = false;
	NextScreen          = NULL;
	NextScreenWithCheck = NULL;
	BlackScreen         = false;

	// init fade
	FadeStartTime = 0;
	FadeEnabled = (sIni.ScreenFade == Switch::On);
	FadeFailed  = false;
	DoneOnShow  = false;

	NextFPSSwap = 0;
	FPSCounter  = 0;
	LastFPS     = 0;

	glGenTextures(2, FadeTex);
	InitFadeTextures();

	// set LastError for OSD to No Error
	OSD_LastError = "No Errors";

	// software cursor default values
	CursorLastMove = 0;
	CursorVisible  = false;
	CursorPressed  = false;
	CursorX        = -1;
	CursorY        = -1;
	CursorFade     = false;
	CursorHiddenByScreen = true;
}

void Display::InitFadeTextures()
{
	TexW = (uint32) Round(std::pow(2.0, std::ceil(Log2(ScreenW / Screens))));
	TexH = (uint32) Round(std::pow(2.0, std::ceil(Log2(ScreenH))));

	for (int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, FadeTex[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TexW, TexH, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	}
}

bool Display::Draw()
{
	for (int screen = 1; screen <= Screens; screen++)
	{
		ScreenAct = screen;
		ScreenX = 0;

		glViewport((screen - 1) * ScreenW / Screens, 0, ScreenW / Screens, ScreenH);

		// OK was pressed on the popup...
		if (CheckOK)
		{
			CheckOK = false;

			// If there's a screen to go to, move to it.
			if (NextScreenWithCheck != NULL)
			{
				NextScreen = NextScreenWithCheck;
				NextScreenWithCheck = NULL;
			}
			// On the end of the game, fade to black before exit.
			else
			{
				BlackScreen = true;
			}
		}

		// No next screen, and not fading to black...
		// Draw the current screen.
		if (NextScreen == NULL
			&& !BlackScreen)
		{
			// ePreDraw.CallHookChain(false);
			CurrentScreen->Draw();
			
			// Popups
			if (UIPopupError != NULL && UIPopupError->Visible)
				UIPopupError->Draw();
			else if (UIPopupInfo != NULL && UIPopupInfo->Visible)
				UIPopupInfo->Draw();
			else if (UIPopupCheck != NULL && UIPopupCheck->Visible)
				UIPopupCheck->Draw();

			FadeStartTime = 0;
			FadeEnabled = (sIni.ScreenFade == Switch::On && !FadeFailed);

			// eDraw.CallHookChain(false);
		}
		else
		{
			// Disable fading if initialization failed
			if (FadeEnabled && FadeFailed)
				FadeEnabled = false;

			// Can we fade now?
			if (FadeEnabled && !FadeFailed)
			{
				// Create fading texture if we're just starting
				if (FadeStartTime == 0)
				{
					// Draw screen that will be faded
					// ePreDraw.CallHookChain(false);
					CurrentScreen->Draw();
					// eDraw.CallHookChain(false);

					// Clear OpenGL errors, otherwise fading might be disabled
					// due to some older errors in previous OpenGL calls
					glGetError();

					uint32 fadeCopyW = ScreenW / Screens,
						fadeCopyH = ScreenH;

					// It is possible that our fade textures are too small after
					// a window resize. In that case, resize the texture to fit.
					if (TexW < fadeCopyW || TexH < fadeCopyH)
						InitFadeTextures();

					// Copy screen to texture
					glBindTexture(GL_TEXTURE_2D, FadeTex[screen - 1]);
					glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (screen - 1) * ScreenW / Screens, 
						0, fadeCopyW, fadeCopyH);

					GLenum glError = glGetError();
					if (glError != GL_NO_ERROR)
					{
						FadeFailed = true;

						// TODO: Do we want to use an error string? I'd hate to want to include/link to glu.h for just that
						// Might be helpful to dump a lookup table later...
						sLog.Error("Display::Draw", "Fading disabled, OpenGL error code: 0x%X", glError);
					}

					if (!BlackScreen 
						&& screen == 1 
						&& !DoneOnShow)
					{
						NextScreen->OnShow();
						DoneOnShow = true;
					}

					// Set fade time once on final screen
					if (screen == Screens)
						FadeStartTime = SDL_GetTicks();
				} // end of create fading texture

				if (!BlackScreen)
				{
					// ePreDraw.CallHookChain(false);
					NextScreen->Draw();
					// eDraw.CallHookChain(false);
				}
				// Draw black screen
				else if (ScreenAct == 1)
				{
					glClearColor(0, 0, 0, 1);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				}

				// and draw old screen over it... slowly fading out
				float fadeStateSquare = 0.0f;
				if (FadeStartTime > 0)
					fadeStateSquare = sqr((float)(SDL_GetTicks() - FadeStartTime) / FADE_DURATION);

				if (fadeStateSquare < 1)
				{
					float	fadeW = ((float)ScreenW / Screens) / (float) TexW,
							fadeH = (float) ScreenH / (float) TexH;

					glBindTexture(GL_TEXTURE_2D, FadeTex[screen - 1]);

					// TODO: check if glTexEnvi() gives any speed improvement
					// glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
					
					glColor4f(1.0f, 1.0f, 1.0f, 1 - fadeStateSquare);

					glEnable(GL_TEXTURE_2D);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glEnable(GL_BLEND);
					glBegin(GL_QUADS);
						glTexCoord2f((0+fadeStateSquare/2)*fadeW, (0+fadeStateSquare/2)*fadeH);
						glVertex2f(0.0f, (float) RenderH);
						glTexCoord2f((0+fadeStateSquare/2)*fadeW, (1-fadeStateSquare/2)*fadeH);
						glVertex2f(0.0f, 0.0f);
						glTexCoord2f((1-fadeStateSquare/2)*fadeW, (1-fadeStateSquare/2)*fadeH);
						glVertex2f((float) RenderW, 0.0f);
						glTexCoord2f((1-fadeStateSquare/2)*fadeW, (0+fadeStateSquare/2)*fadeH);
						glVertex2f((float) RenderW, (float) RenderH);
					glEnd();
					glDisable(GL_BLEND);
					glDisable(GL_TEXTURE_2D);

					// reset to default
					// glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULE);
				}
			}
			// There is no need to init next screen if it is a black screen
			else if (!BlackScreen)
			{
				NextScreen->OnShow();
			}

			// Fade out complete (if it was even fading to begin with...)
			if (((FadeStartTime + FADE_DURATION < SDL_GetTicks()
				|| (!FadeEnabled || FadeFailed))
				&& (screen == Screens)))
			{
				FadeStartTime = 0;
				DoneOnShow = false;
				CurrentScreen->OnHide();
				CurrentScreen->ShowFinish = false;
				CurrentScreen = NextScreen;
				NextScreen = NULL;

				if (BlackScreen)
					return false;

				CurrentScreen->OnShowFinish();
				CurrentScreen->ShowFinish = true;
			}
		}

		// Draw OSD only on first screen if debug mode is enabled
		if ((sIni.Debug || Params.Debug)
			&& screen == 1)
			DrawDebugInformation();

		if (!BlackScreen)
			DrawCursor();
	}

	return true;
}

// called by MoveCursor and OnMouseButton to update last move and start fade in
void Display::UpdateCursorFade()
{
	uint32 ticks = SDL_GetTicks();

	// fade in on movement (or button press) if not first movement
	if (!CursorVisible && CursorLastMove != 0)
	{
		if (CursorFade)
			CursorLastMove -= (uint32_t) Round(1.0 * (CURSOR_FADE_IN_TIME * (1 - (ticks - CursorLastMove)) / CURSOR_FADE_OUT_TIME));
		else
			CursorLastMove = ticks;

		CursorVisible = true;
		CursorFade = true;
	}
	else if (!CursorFade)
	{
		CursorLastMove = ticks;
	}
}

void Display::MoveCursor(float x, float y)
{
	if (sIni.Mouse != 2)
		return;

	CursorX = x;
	CursorY = y;

	UpdateCursorFade();
}

void Display::OnMouseButton(bool pressed)
{
	if (sIni.Mouse != 2)
		return;

	CursorPressed = pressed;
	UpdateCursorFade();
}

// Draws software cursor
void Display::DrawCursor()
{
	float Alpha;
	uint32 Ticks;
	float DrawX;

	// draw software cursor
	if ((sIni.Mouse == 2) && ((Screens == 1) || ((ScreenAct - 1) == (Round(CursorX + 16) / RenderW))))
	{
		Ticks = SDL_GetTicks();

		// start fade out after 5 secs w/o activity
		if ((CursorVisible) && (CursorLastMove + CURSOR_AUTOHIDE_TIME <= Ticks))
		{
			CursorVisible = false;
			CursorLastMove = Ticks;
			CursorFade = true;
		}

		// fading
		if (CursorFade)
		{
			// fade in
			if (CursorVisible)
			{
				if (CursorLastMove + CURSOR_FADE_IN_TIME <= Ticks)
					CursorFade = false;
				else
					Alpha = (float) (std::sin((Ticks - CursorLastMove) * 0.5 * M_PI / CURSOR_FADE_IN_TIME) * 0.7);
			}
			// fade out
			else
			{
				if (CursorLastMove + CURSOR_FADE_OUT_TIME <= Ticks)
					CursorFade = false;
				else
					Alpha = (float) (std::cos((Ticks - CursorLastMove) * 0.5 * M_PI / CURSOR_FADE_OUT_TIME) * 0.7);
			}
		}

		// no else if here because we may turn off fade in if block
		if (!CursorFade)
		{
			if (CursorVisible)
				Alpha = 0.7f; // alpha when cursor visible and not fading
			else
				Alpha = 0;   // alpha when cursor is hidden
		}

		if (Alpha > 0 && !CursorHiddenByScreen)
		{
			DrawX = CursorX;
			if (ScreenAct == 2)
				DrawX -= RenderW;

			glColor4f(1, 1, 1, Alpha);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);

			if (CursorPressed && TexCursorPressed.TexNum > 0)
				glBindTexture(GL_TEXTURE_2D, TexCursorPressed.TexNum);
			else
				glBindTexture(GL_TEXTURE_2D, TexCursorUnpressed.TexNum);

			glBegin(GL_QUADS);
				glTexCoord2i(0, 0);
				glVertex2f(DrawX, CursorY);

				glTexCoord2f(0, 1);
				glVertex2f(DrawX, CursorY + 32);

				glTexCoord2f(1, 1);
				glVertex2f(DrawX + 32, CursorY + 32);

				glTexCoord2f(1, 0);
				glVertex2f(DrawX + 32, CursorY);
			glEnd();

			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);
		}
	}
}

void Display::DrawDebugInformation()
{
	// White background for information
	glEnable(GL_BLEND);
	glColor4f(1, 1, 1, 0.5);
	glBegin(GL_QUADS);
		glVertex2i(RenderH + 90, 44);
		glVertex2i(RenderH + 90, 0);
		glVertex2i(RenderW, 0);
		glVertex2i(RenderW, 44);
	glEnd();
	glDisable(GL_BLEND);

	// set font specs
	SetFontStyle(ftNormal);
	SetFontSize(21);
	SetFontItalic(false);
	glColor4f(0, 0, 0, 1);

	// calculate fps
	uint32 Ticks = SDL_GetTicks();
	if (Ticks >= NextFPSSwap)
	{
		LastFPS = FPSCounter * 4;
		FPSCounter = 0;
		NextFPSSwap = Ticks + 250;
	}

	++FPSCounter;

	/* draw text */

	// fps
	SetFontPos(695, 0);
	glPrint("FPS: %d", LastFPS);

	// rspeed
	SetFontPos(695, 13);
	glPrint("RSpeed: %d", (uint32) ceil(1000 * GetTimeMid()));

	// lasterror
	SetFontPos(695, 26);
	glColor4f(1, 0, 0, 1);
	glPrint(OSD_LastError);

	glColor4f(1, 1, 1, 1);
}

bool Display::ParseInput(uint32 pressedKey, SDL_Keycode keyCode, bool pressedDown)
{
	if (NextScreen != NULL)
		return NextScreen->ParseInput(pressedKey, keyCode, pressedDown);
	else if (CurrentScreen != NULL)
		return CurrentScreen->ParseInput(pressedKey, keyCode, pressedDown);

	return true;
}

bool Display::ParseMouse(int mouseButton, bool btnDown, float x, float y)
{
	if (CurrentScreen != NULL)
		return CurrentScreen->ParseMouse(mouseButton, btnDown, x, y);

	return true;
}

bool Display::ParseTextInput(SDL_Event * event)
{
	if (CurrentScreen != NULL)
		return CurrentScreen->ParseTextInput(event);

	return true;
}

void Display::SetCursor()
{
	int cursor = 0;

	// Hide cursor
	if (CurrentScreen != UISing
		|| CursorHiddenByScreen)
	{
		// Show SDL (OS) cursor in window mode even when mouse support is off
		if ((sIni.Mouse == 0 && !Fullscreen)
			// Show SDL (OS) cursor when hardware cursor is selected.
			|| sIni.Mouse == 1)
			cursor = 1;

		if (sIni.Mouse != 2)
			CursorHiddenByScreen = false;
	}
	else if (sIni.Mouse != 2)
	{
		CursorHiddenByScreen = true;
	}

	SDL_ShowCursor(cursor);

	if (sIni.Mouse == 2)
	{
		// Show software cursor
		if (CursorHiddenByScreen)
		{
			CursorHiddenByScreen = false;
			CursorVisible = false;
			CursorFade = false;
		}
		// Hide software cursor in sing screen
		else if (CurrentScreen != UISing)
		{
			CursorHiddenByScreen = true;
			CursorVisible = false;
			CursorFade = false;
		}
	}
}

Display::~Display()
{
	glDeleteTextures(2, FadeTex);
}
