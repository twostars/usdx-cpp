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

#ifndef _LANGUAGE_H
#define _LANGUAGE_H
#pragma once

#define DEFAULT_LANGUAGE _T("English")

typedef std::vector<tstring> LanguageSet;
typedef std::map<tstring, tstring> LanguageEntryMap;

class Language : public Singleton<Language>
{
public:
	Language();
	void LoadList();
	LanguageSet& GetLanguageSet() { return _langSet; }
	void ChangeLanguage(const tstring& language);
	uint32 GetLanguageID(const tstring& language);
	const tstring& GetLanguageByID(uint32 id);
	void AddConst(const tstring& id, const tstring& text);
	const tstring Translate(const tstring& id);
	const tstring Translate(const TCHAR* id);
	void TranslateOptionValues();
	~Language();

private:
	LanguageSet _langSet;
	LanguageEntryMap _translatedLanguageNames;

	LanguageEntryMap _langEntryMap;
	LanguageEntryMap _langConstEntryMap;
};

#define sLanguage (Language::getSingleton())

extern tstring IDifficultyTranslated[3];
extern tstring ITabsTranslated[2];

extern tstring ISortingTranslated[7];

extern tstring IDebugTranslated[2];

extern tstring IFullScreenTranslated[2];
extern tstring IVisualizerTranslated[3];

extern tstring IBackgroundMusicTranslated[2];
extern tstring ISingWindowTranslated[2];

extern tstring IOscilloscopeTranslated[2];

extern tstring ISpectrumTranslated[2];
extern tstring ISpectrographTranslated[2];
extern tstring IMovieSizeTranslated[3];
extern tstring IVideoPreviewTranslated[2];
extern tstring IVideoEnabledTranslated[2];

extern tstring IClickAssistTranslated[2];
extern tstring IBeatClickTranslated[2];
extern tstring ISavePlaybackTranslated[2];

extern tstring IVoicePassthroughTranslated[2];

extern tstring ISyncToTranslated[3];

extern tstring IAudioOutputBufferSizeTranslated[10];
extern tstring IAudioInputBufferSizeTranslated[10];

extern tstring IPreviewVolumeTranslated[11];
extern tstring IPreviewFadingTranslated[6];

extern tstring ILyricsFontTranslated[3];
extern tstring ILyricsEffectTranslated[5];
extern tstring INoteLinesTranslated[2];

extern tstring IColorTranslated[9];

// Advanced
extern tstring ILoadAnimationTranslated[2];
extern tstring IEffectSingTranslated[2];
extern tstring IScreenFadeTranslated[2];
extern tstring IAskBeforeDelTranslated[2];
extern tstring IOnSongClickTranslated[3];

extern tstring ILineBonusTranslated[2];
extern tstring IPartyPopupTranslated[2];

extern tstring IJoypadTranslated[2];
extern tstring IMouseTranslated[3];

// Recording options
extern tstring IChannelPlayerTranslated[7];
extern tstring IMicBoostTranslated[4];

#endif
