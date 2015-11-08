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

#define DEFAULT_LANGUAGE "English"

typedef std::map<std::string, std::string> LanguageEntryMap;

class Language : public Singleton<Language>
{
public:
	Language();
	void LoadList();
	OptionList& GetLanguageSet() { return _langSet; }
	void ChangeLanguage(const std::string& language);
	uint32 GetLanguageID(const std::string& language) { return GetListIndex(_langSet, language); }
	const std::string& GetLanguageByID(uint32 id) { return _langSet[id]; }
	void AddConst(const std::string& id, const std::string& text);
	const std::string Translate(const std::string& id);
	const std::string Translate(const char* id);
	void TranslateOptionValues();
	~Language();

private:
	OptionList _langSet;
	LanguageEntryMap _translatedLanguageNames;

	LanguageEntryMap _langEntryMap;
	LanguageEntryMap _langConstEntryMap;
};

#define sLanguage (Language::getSingleton())

extern std::string IDifficultyTranslated[3];
extern std::string ITabsTranslated[2];

extern std::string ISortingTranslated[7];

extern std::string IDebugTranslated[2];

extern std::string IFullScreenTranslated[2];
extern std::string IVisualizerTranslated[3];

extern std::string IBackgroundMusicTranslated[2];
extern std::string ISingWindowTranslated[2];

extern std::string IOscilloscopeTranslated[2];

extern std::string ISpectrumTranslated[2];
extern std::string ISpectrographTranslated[2];
extern std::string IMovieSizeTranslated[3];
extern std::string IVideoPreviewTranslated[2];
extern std::string IVideoEnabledTranslated[2];

extern std::string IClickAssistTranslated[2];
extern std::string IBeatClickTranslated[2];
extern std::string ISavePlaybackTranslated[2];

extern std::string IVoicePassthroughTranslated[2];

extern std::string ISyncToTranslated[3];

extern std::string IAudioOutputBufferSizeTranslated[10];
extern std::string IAudioInputBufferSizeTranslated[10];

extern std::string IPreviewVolumeTranslated[11];
extern std::string IPreviewFadingTranslated[6];

extern std::string ILyricsFontTranslated[3];
extern std::string ILyricsEffectTranslated[5];
extern std::string INoteLinesTranslated[2];

extern std::string IColorTranslated[9];

// Advanced
extern std::string ILoadAnimationTranslated[2];
extern std::string IEffectSingTranslated[2];
extern std::string IScreenFadeTranslated[2];
extern std::string IAskBeforeDelTranslated[2];
extern std::string IOnSongClickTranslated[3];

extern std::string ILineBonusTranslated[2];
extern std::string IPartyPopupTranslated[2];

extern std::string IJoypadTranslated[2];
extern std::string IMouseTranslated[3];

// Recording options
extern std::string IChannelPlayerTranslated[7];
extern std::string IMicBoostTranslated[4];

#endif
