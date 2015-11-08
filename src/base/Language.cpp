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
#include "Language.h"
#include "PathUtils.h"
#include "Log.h"
#include "Ini.h"

using namespace boost::filesystem;

initialiseSingleton(Language);

extern path LanguagesPath;

/*
 * Translated options
 */

std::string IDifficultyTranslated[]       = { "Easy", "Medium", "Hard" };
std::string ITabsTranslated[]             = { "Off", "On" };

std::string ISortingTranslated[]          = { "Edition", "Genre", "Language", "Folder", "Title", "Artist", "Artist2" };

std::string IDebugTranslated[]            = { "Off", "On" }; 

std::string IFullScreenTranslated[]       = { "Off", "On" };
std::string IVisualizerTranslated[]       = { "Off", "WhenNoVideo", "On" };

std::string IBackgroundMusicTranslated[]  = { "Off", "On" };
std::string ISingWindowTranslated[]       = { "Small", "Big" };

std::string IOscilloscopeTranslated[]     = { "Off", "On" };

std::string ISpectrumTranslated[]         = { "Off", "On" };
std::string ISpectrographTranslated[]     = { "Off", "On" };
std::string IMovieSizeTranslated[]        = { "Half", "Full [Vid]", "Full [BG+Vid]" };
std::string IVideoPreviewTranslated[]     = { "Off", "On" };
std::string IVideoEnabledTranslated[]     = { "Off", "On" };

std::string IClickAssistTranslated[]      = { "Off", "On" };
std::string IBeatClickTranslated[]        = { "Off", "On" };
std::string ISavePlaybackTranslated[]     = { "Off", "On" };

std::string IVoicePassthroughTranslated[] = { "Off", "On" };

std::string ISyncToTranslated[]           = { "Music", "Lyrics", "Off" };

std::string 
	IAudioOutputBufferSizeTranslated[] = { "Auto", "256",  "512",   "1024",  "2048", 
	                                       "4096", "8192", "16384", "32768", "65536" };

std::string
	IAudioInputBufferSizeTranslated[] = { "Auto", "256",  "512",   "1024",  "2048", 
                                           "4096", "8192", "16384", "32768", "65536" };

std::string IPreviewVolumeTranslated[]    = { "Off", "10%", "20%", "30%", "40%", "50%", 
	                                      "60%", "70%", "80%", "90%", "100%" };

std::string IPreviewFadingTranslated[]    = { "Off", "1 Sec", "2 Secs", "3 Secs", "4 Secs", "5 Secs" };

std::string ILyricsFontTranslated[]       = { "Plain", "OLine1", "OLine2" };
std::string ILyricsEffectTranslated[]     = { "Simple", "Zoom", "Slide", "Ball", "Shift" };
std::string INoteLinesTranslated[]        = { "Off", "On" };

std::string IColorTranslated[]            = { "Blue", "Green", "Pink", "Red", "Violet", 
	                                      "Orange", "Yellow", "Brown", "Black" };

// Advanced
std::string ILoadAnimationTranslated[]    = { "Off", "On" };
std::string IEffectSingTranslated[]       = { "Off", "On" };
std::string IScreenFadeTranslated[]       = { "Off", "On" };
std::string IAskBeforeDelTranslated[]     = { "Off", "On" };
std::string IOnSongClickTranslated[]      = { "Sing", "Select Players", "Open Menu" };

std::string ILineBonusTranslated[]        = { "Off", "On" };
std::string IPartyPopupTranslated[]       = { "Off", "On" };

std::string IJoypadTranslated[]           = { "Off", "On" };
std::string IMouseTranslated[]            = { "Off", "Hardware Cursor", "Software Cursor" };

// Recording options
std::string IChannelPlayerTranslated[]    = { "Off", "1", "2", "3", "4", "5", "6" };
std::string IMicBoostTranslated[]         = { "Off", "+6dB", "+12dB", "+18dB" };

Language::Language()
{
	LoadList();
}

void Language::LoadList()
{
	directory_iterator end;

	try
	{
		for (directory_iterator itr(LanguagesPath); itr != end; ++itr)
		{
			const path& p = itr->path();
			if (is_directory(p)
				|| !p.has_extension()
				|| p.extension() != ".ini")
				continue;

			std::string languageName = p.filename().stem().generic_string();

			// insert the new language name to the set
			_langSet.push_back(languageName);
		}
	}
	catch (filesystem_error)
	{
		sLog.Critical(
			"Language::LoadList",
			"Could not access %s", 
			LanguagesPath.generic_string().c_str());
	}

	if (_langSet.empty())
		sLog.Critical("Could not load any language file.");

	OptionList::const_iterator itr = std::find(_langSet.begin(), _langSet.end(), DEFAULT_LANGUAGE);
	if (itr == _langSet.end())
		sLog.Critical("Default language (" DEFAULT_LANGUAGE ") does not exist.");
}

void Language::ChangeLanguage(const std::string& language)
{
	const char * sectionName = "Text";

	CSimpleIniA ini(true);
	path iniPath(LanguagesPath);
	iniPath /= language;
	iniPath.replace_extension(".ini");

	SI_Error result = ini.LoadFile(iniPath.native().c_str());

	if (result != SI_OK)
		sLog.Critical("Change language", "Failed to set language to %s.", language.c_str());

	CSimpleIniA::TNamesDepend keys;
	ini.GetAllKeys(sectionName, keys);
	for (CSimpleIniA::TNamesDepend::const_iterator itr = keys.begin(); itr != keys.end(); ++itr)
		_langEntryMap[itr->pItem] = ini.GetValue(sectionName, itr->pItem);

	// Update language name for config
	sIni.LanguageName = language;
	sIni.Language = GetLanguageID(language);

	// Translate & cache option values so we can avoid
	// looking them up on-demand.
	TranslateOptionValues();
}

void Language::AddConst(const std::string& id, const std::string& text)
{
	_langConstEntryMap[id] = text;
}

const std::string Language::Translate(const std::string& id)
{
	LanguageEntryMap::iterator itr;

	// Does this ID have a constant override?
	itr = _langConstEntryMap.find(id);
	if (itr != _langConstEntryMap.end())
		return itr->second;

	// Does this ID exist in the language map?
	itr = _langEntryMap.find(id);
	if (itr != _langEntryMap.end())
		return itr->second;

	// Doesn't exist anywhere? Use the ID.
	// NOTE: Official USDX uses a language map for (English) defaults.
	// This seems a little messy, so we'll just encourage fixing any missing language references
	// by not translating at all, and instead providing the text ID.
	return id;
}

const std::string Language::Translate(const char * id)
{
	std::string tmp = id;
	return Translate(tmp);
}

void Language::TranslateOptionValues()
{
	// Translate language names.
	_translatedLanguageNames.clear();
	for (OptionList::const_iterator itr = _langSet.begin(); itr != _langSet.end(); ++itr)
		_translatedLanguageNames[*itr] = Translate("OPTION_VALUE_" + *itr);

	std::string switchOff = Translate("OPTION_VALUE_OFF");
	std::string switchOn  = Translate("OPTION_VALUE_ON");

	// Now translate the option values.
	IDifficultyTranslated[DifficultyType::Easy]           = Translate("OPTION_VALUE_EASY");
	IDifficultyTranslated[DifficultyType::Medium]         = Translate("OPTION_VALUE_MEDIUM");
	IDifficultyTranslated[DifficultyType::Hard]           = Translate("OPTION_VALUE_HARD");

	ITabsTranslated[Switch::Off]                          = switchOff;
	ITabsTranslated[Switch::On]                           = switchOn;

	ISortingTranslated[SortingType::Edition]              = Translate("OPTION_VALUE_EDITION");
	ISortingTranslated[SortingType::Genre]                = Translate("OPTION_VALUE_GENRE");
	ISortingTranslated[SortingType::Language]             = Translate("OPTION_VALUE_LANGUAGE");
	ISortingTranslated[SortingType::Folder]               = Translate("OPTION_VALUE_FOLDER");
	ISortingTranslated[SortingType::Title]                = Translate("OPTION_VALUE_TITLE");
	ISortingTranslated[SortingType::Artist]               = Translate("OPTION_VALUE_ARTIST");
	ISortingTranslated[SortingType::Artist2]              = Translate("OPTION_VALUE_ARTIST2");

	IDebugTranslated[Switch::Off]                         = switchOff;
	IDebugTranslated[Switch::On]                          = switchOn;

	IFullScreenTranslated[Switch::Off]                    = switchOff;
	IFullScreenTranslated[Switch::On]                     = switchOn;

	IVisualizerTranslated[VisualizerOption::Off]          = switchOff;
	IVisualizerTranslated[VisualizerOption::WhenNoVideo]  = Translate("OPTION_VALUE_WHENNOVIDEO");
	IVisualizerTranslated[VisualizerOption::On]           = switchOn;

	IBackgroundMusicTranslated[Switch::Off]               = switchOff;
	IBackgroundMusicTranslated[Switch::On]                = switchOn;

	ISingWindowTranslated[SingWindowType::Small]          = Translate("OPTION_VALUE_SMALL");
	ISingWindowTranslated[SingWindowType::Big]            = Translate("OPTION_VALUE_BIG");

	IOscilloscopeTranslated[Switch::Off]                  = switchOff;
	IOscilloscopeTranslated[Switch::On]                   = switchOn;

	ISpectrumTranslated[Switch::Off]                      = switchOff;
	ISpectrumTranslated[Switch::On]                       = switchOn;

	ISpectrographTranslated[Switch::Off]                  = switchOff;
	ISpectrographTranslated[Switch::On]                   = switchOn;

	IMovieSizeTranslated[0]                               = Translate("OPTION_VALUE_HALF");
	IMovieSizeTranslated[1]                               = Translate("OPTION_VALUE_FULL_VID");
	IMovieSizeTranslated[2]                               = Translate("OPTION_VALUE_FULL_VID_BG");

	IVideoPreviewTranslated[Switch::Off]                  = switchOff;
	IVideoPreviewTranslated[Switch::On]                   = switchOn;

	IVideoEnabledTranslated[Switch::Off]                  = switchOff;
	IVideoEnabledTranslated[Switch::On]                   = switchOn;

	IClickAssistTranslated[Switch::Off]                   = switchOff;
	IClickAssistTranslated[Switch::On]                    = switchOn;

	IBeatClickTranslated[Switch::Off]                     = switchOff;
	IBeatClickTranslated[Switch::On]                      = switchOn;

	ISavePlaybackTranslated[Switch::Off]                  = switchOff;
	ISavePlaybackTranslated[Switch::On]                   = switchOn;

	IVoicePassthroughTranslated[Switch::Off]              = switchOff;
	IVoicePassthroughTranslated[Switch::On]               = switchOn;

	ISyncToTranslated[SyncToType::Music]                  = Translate("OPTION_VALUE_MUSIC");
	ISyncToTranslated[SyncToType::Lyrics]                 = Translate("OPTION_VALUE_LYRICS");
	ISyncToTranslated[SyncToType::Off]                    = switchOff;

	ILyricsFontTranslated[LyricsFontType::Plain]          = Translate("OPTION_VALUE_PLAIN");
	ILyricsFontTranslated[LyricsFontType::OLine1]         = Translate("OPTION_VALUE_OLINE1");
	ILyricsFontTranslated[LyricsFontType::OLine2]         = Translate("OPTION_VALUE_OLINE2");

	ILyricsEffectTranslated[LyricsEffectType::Simple]     = Translate("OPTION_VALUE_SIMPLE");
	ILyricsEffectTranslated[LyricsEffectType::Zoom]       = Translate("OPTION_VALUE_ZOOM");
	ILyricsEffectTranslated[LyricsEffectType::Slide]      = Translate("OPTION_VALUE_SLIDE");
	ILyricsEffectTranslated[LyricsEffectType::Ball]       = Translate("OPTION_VALUE_BALL");
	ILyricsEffectTranslated[LyricsEffectType::Shift]      = Translate("OPTION_VALUE_SHIFT");

	INoteLinesTranslated[Switch::Off]                     = switchOff;
	INoteLinesTranslated[Switch::On]                      = switchOn;

	IColorTranslated[Color::Blue]                         = Translate("OPTION_VALUE_BLUE");
	IColorTranslated[Color::Green]                        = Translate("OPTION_VALUE_GREEN");
	IColorTranslated[Color::Pink]                         = Translate("OPTION_VALUE_PINK");
	IColorTranslated[Color::Red]                          = Translate("OPTION_VALUE_RED");
	IColorTranslated[Color::Violet]                       = Translate("OPTION_VALUE_VIOLET");
	IColorTranslated[Color::Orange]                       = Translate("OPTION_VALUE_ORANGE");
	IColorTranslated[Color::Yellow]                       = Translate("OPTION_VALUE_YELLOW");
	IColorTranslated[Color::Brown]                        = Translate("OPTION_VALUE_BROWN");
	IColorTranslated[Color::Black]                        = Translate("OPTION_VALUE_BLACK");

	// Advanced
	ILoadAnimationTranslated[Switch::Off]                 = switchOff;
	ILoadAnimationTranslated[Switch::On]                  = switchOn;

	IEffectSingTranslated[Switch::Off]                    = switchOff;
	IEffectSingTranslated[Switch::On]                     = switchOn;

	IScreenFadeTranslated[Switch::Off]                    = switchOff;
	IScreenFadeTranslated[Switch::On]                     = switchOn;

	IAskBeforeDelTranslated[Switch::Off]                  = switchOff;
	IAskBeforeDelTranslated[Switch::On]                   = switchOn;

	IOnSongClickTranslated[SongClickType::Sing]           = Translate("OPTION_VALUE_SING");
	IOnSongClickTranslated[SongClickType::SelectPlayers]  = Translate("OPTION_VALUE_SELECT_PLAYERS");
	IOnSongClickTranslated[SongClickType::OpenMenu]       = Translate("OPTION_VALUE_OPEN_MENU");

	ILineBonusTranslated[Switch::Off]                     = switchOff;
	ILineBonusTranslated[Switch::On]                      = switchOn;
 
	IPartyPopupTranslated[Switch::Off]                    = switchOff;
	IPartyPopupTranslated[Switch::On]                     = switchOn;

	IJoypadTranslated[Switch::Off]                        = switchOff;
	IJoypadTranslated[Switch::On]                         = switchOn;

	IMouseTranslated[0]                                   = switchOff;
	IMouseTranslated[1]                                   = Translate("OPTION_VALUE_HARDWARE_CURSOR");
	IMouseTranslated[2]                                   = Translate("OPTION_VALUE_SOFTWARE_CURSOR");

	IAudioOutputBufferSizeTranslated[0]                   = Translate("OPTION_VALUE_AUTO");
	IAudioInputBufferSizeTranslated[0]                    = Translate("OPTION_VALUE_AUTO");

	// Song Preview
	IPreviewVolumeTranslated[Switch::Off]                 = switchOff;
	IPreviewFadingTranslated[Switch::Off]                 = switchOff;
	IPreviewFadingTranslated[1]                           = "1 " + Translate("OPTION_VALUE_SEC");
	IPreviewFadingTranslated[2]                           = "2 " + Translate("OPTION_VALUE_SECS");
	IPreviewFadingTranslated[3]                           = "3 " + Translate("OPTION_VALUE_SECS");
	IPreviewFadingTranslated[4]                           = "4 " + Translate("OPTION_VALUE_SECS");
	IPreviewFadingTranslated[5]                           = "5 " + Translate("OPTION_VALUE_SECS");

	// Recording options
	IChannelPlayerTranslated[Switch::Off]                 = switchOff;
	IMicBoostTranslated[Switch::Off]                      = switchOff;
}

Language::~Language()
{
}
