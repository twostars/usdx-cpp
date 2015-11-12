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
#include "../base/Language.h"
#include "../base/Music.h"
#include "../menu/Menu.h"
#include "../base/Themes.h"
#include "ScreenOptions.h"
#include "ScreenOptionsGraphics.h"

ScreenOptionsGraphics::ScreenOptionsGraphics() : Menu()
{
	ThemeOptionsGraphics * theme = sThemes.OptionsGraphics;
	LoadFromTheme(theme);

	OptionList& resolutionList = sIni.ResolutionNameList;
	theme->SelectResolution.ShowArrows = true;
	theme->SelectResolution.OneItemOnly = true;
	AddSelectSlide(theme->SelectResolution, (Uint32 *)&sIni.SelectedResolution, &resolutionList[0], resolutionList.size());

	theme->SelectFullscreen.ShowArrows = true;
	theme->SelectFullscreen.OneItemOnly = true;
	AddSelectSlide(theme->SelectFullscreen, (Uint32 *)&sIni.FullScreen, IFullScreenTranslated, SDL_arraysize(IFullScreenTranslated));

	theme->SelectDepth.ShowArrows = true;
	theme->SelectDepth.OneItemOnly = true;
	AddSelectSlide(theme->SelectDepth, (Uint32 *)&sIni.Depth, IDepth, SDL_arraysize(IDepth));

	theme->SelectVisualizer.ShowArrows = true;
	theme->SelectVisualizer.OneItemOnly = true;
	AddSelectSlide(theme->SelectVisualizer, (Uint32 *)&sIni.VisualizerOption, IVisualizerTranslated, SDL_arraysize(IVisualizerTranslated));

	theme->SelectOscilloscope.ShowArrows = true;
	theme->SelectOscilloscope.OneItemOnly = true;
	AddSelectSlide(theme->SelectOscilloscope, (Uint32 *)&sIni.Oscilloscope, IOscilloscopeTranslated, SDL_arraysize(IOscilloscopeTranslated));

	theme->SelectMovieSize.ShowArrows = true;
	theme->SelectMovieSize.OneItemOnly = true;
	AddSelectSlide(theme->SelectMovieSize, (Uint32 *)&sIni.MovieSize, IMovieSizeTranslated, SDL_arraysize(IMovieSizeTranslated));

	AddButton(theme->ButtonExit);
	if (Buttons[0].Texts.empty())
		AddButtonText(20.0f, 5.0f, sThemes.Options->Description[7]);
}

bool ScreenOptionsGraphics::ParseInput(Uint32 pressedKey, SDL_Keycode keyCode, bool pressedDown)
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
			if (SelInteraction == 6)
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
			if (SelInteraction >= 0 && SelInteraction <= 5)
			{
				if (pressedKey == SDLK_RIGHT
					? InteractInc()
					: InteractDec())
				sSoundLib.PlaySound(SoundOption);
			}
			break;
	}

	return true;
}

void ScreenOptionsGraphics::OnShow()
{
	Menu::OnShow();
	OldResolution = sIni.SelectedResolution;
	SetInteraction(0);
}

void ScreenOptionsGraphics::SaveAndReturn()
{
	sIni.SaveGraphicsSettings();
	sIni.SaveToFile();

	// If we've changed the resolution...
	if (OldResolution != sIni.SelectedResolution)
	{
		// TODO: Change the resolution.
		// Currently this will invalidate all textures on Windows/Mac OSX (Linux is fine).
	}

	sSoundLib.PlaySound(SoundBack);
	FadeTo(UIOptions);
}
