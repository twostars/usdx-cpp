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
#include "ScreenOptionsSound.h"

void OnBackgroundMusicChanged(Uint32 oldValue, Uint32 newValue);

ScreenOptionsSound::ScreenOptionsSound() : Menu()
{
	ThemeOptionsSound * theme = sThemes.OptionsSound;

	LoadFromTheme(theme);

	theme->SelectSlideVoicePassthrough.ShowArrows = true;
	theme->SelectSlideVoicePassthrough.OneItemOnly = true;
	int passthrough = AddSelectSlide(theme->SelectSlideVoicePassthrough, (Uint32 *)&sIni.VoicePassthrough, IVoicePassthroughTranslated, SDL_arraysize(IVoicePassthroughTranslated));

	theme->SelectBackgroundMusic.ShowArrows = true;
	theme->SelectBackgroundMusic.OneItemOnly = true;
	int bgMusic = AddSelectSlide(theme->SelectBackgroundMusic, (Uint32 *)&sIni.BackgroundMusic, IBackgroundMusicTranslated, SDL_arraysize(IBackgroundMusicTranslated));
	SelectSlides[bgMusic].OnValueChanged = &OnBackgroundMusicChanged;

	theme->SelectClickAssist.ShowArrows = true;
	theme->SelectClickAssist.OneItemOnly = true;
	int clickAssist = AddSelectSlide(theme->SelectClickAssist, (Uint32 *)&sIni.ClickAssist, IClickAssistTranslated, SDL_arraysize(IClickAssistTranslated));

	theme->SelectBeatClick.ShowArrows = true;
	theme->SelectBeatClick.OneItemOnly = true;
	int beatClick = AddSelectSlide(theme->SelectBeatClick, (Uint32 *)&sIni.BeatClick, IBeatClickTranslated, SDL_arraysize(IBeatClickTranslated));

	theme->SelectSlidePreviewVolume.ShowArrows = true;
	theme->SelectSlidePreviewVolume.OneItemOnly = true;
	int previewVolume = AddSelectSlide(theme->SelectSlidePreviewVolume, (Uint32 *)&sIni.PreviewVolume, IPreviewVolumeTranslated, SDL_arraysize(IPreviewVolumeTranslated));

	theme->SelectSlidePreviewFading.ShowArrows = true;
	theme->SelectSlidePreviewFading.OneItemOnly = true;
	int previewFading = AddSelectSlide(theme->SelectSlidePreviewFading, (Uint32 *)&sIni.PreviewFading, IPreviewFadingTranslated, SDL_arraysize(IPreviewFadingTranslated));

	AddButton(theme->ButtonExit);
	if (Buttons[0].Texts.empty())
		AddButtonText(20.0f, 5.0f, sThemes.Options->Description[7]);

	SetInteraction(0);
}

bool ScreenOptionsSound::ParseInput(Uint32 pressedKey, SDL_Keycode keyCode, bool pressedDown)
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
			if (SelInteraction >= 0 && SelInteraction <= 6)
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

void ScreenOptionsSound::OnShow()
{
	Menu::OnShow();
	SetInteraction(0);
}

void OnBackgroundMusicChanged(Uint32 oldValue, Uint32 newValue)
{
	// Turn on/off background music if set.
	if (sIni.BackgroundMusic == eSwitch::On)
		sSoundLib.StartBgMusic();
	else
		sSoundLib.PauseBgMusic();
}

void ScreenOptionsSound::SaveAndReturn()
{
	sIni.Save();
	FadeTo(UIOptions, SoundBack);
}
