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

tstring IDifficultyTranslated[]       = { _T("Easy"), _T("Medium"), _T("Hard") };
tstring ITabsTranslated[]             = { _T("Off"), _T("On") };

tstring ISortingTranslated[]          = { _T("Edition"), _T("Genre"), _T("Language"), _T("Folder"), _T("Title"), _T("Artist"), _T("Artist2") };

tstring IDebugTranslated[]            = { _T("Off"), _T("On") }; 

tstring IFullScreenTranslated[]       = { _T("Off"), _T("On") };
tstring IVisualizerTranslated[]       = { _T("Off"), _T("WhenNoVideo"), _T("On") };

tstring IBackgroundMusicTranslated[]  = { _T("Off"), _T("On") };
tstring ISingWindowTranslated[]       = { _T("Small"), _T("Big") };

tstring IOscilloscopeTranslated[]     = { _T("Off"), _T("On") };

tstring ISpectrumTranslated[]         = { _T("Off"), _T("On") };
tstring ISpectrographTranslated[]     = { _T("Off"), _T("On") };
tstring IMovieSizeTranslated[]        = { _T("Half"), _T("Full [Vid]"), _T("Full [BG+Vid]") };
tstring IVideoPreviewTranslated[]     = { _T("Off"), _T("On") };
tstring IVideoEnabledTranslated[]     = { _T("Off"), _T("On") };

tstring IClickAssistTranslated[]      = { _T("Off"), _T("On") };
tstring IBeatClickTranslated[]        = { _T("Off"), _T("On") };
tstring ISavePlaybackTranslated[]     = { _T("Off"), _T("On") };

tstring IVoicePassthroughTranslated[] = { _T("Off"), _T("On") };

tstring ISyncToTranslated[]           = { _T("Music"), _T("Lyrics"), _T("Off") };

tstring 
	IAudioOutputBufferSizeTranslated[] = { _T("Auto"), _T("256"),  _T("512"),   _T("1024"),  _T("2048"), 
	                                       _T("4096"), _T("8192"), _T("16384"), _T("32768"), _T("65536") };

tstring
	IAudioInputBufferSizeTranslated[] = { _T("Auto"), _T("256"),  _T("512"),   _T("1024"),  _T("2048"), 
                                           _T("4096"), _T("8192"), _T("16384"), _T("32768"), _T("65536") };

tstring IPreviewVolumeTranslated[]    = { _T("Off"), _T("10%"), _T("20%"), _T("30%"), _T("40%"), _T("50%"), 
	                                      _T("60%"), _T("70%"), _T("80%"), _T("90%"), _T("100%") };

tstring IPreviewFadingTranslated[]    = { _T("Off"), _T("1 Sec"), _T("2 Secs"), _T("3 Secs"), _T("4 Secs"), _T("5 Secs") };

tstring ILyricsFontTranslated[]       = { _T("Plain"), _T("OLine1"), _T("OLine2") };
tstring ILyricsEffectTranslated[]     = { _T("Simple"), _T("Zoom"), _T("Slide"), _T("Ball"), _T("Shift") };
tstring INoteLinesTranslated[]        = { _T("Off"), _T("On") };

tstring IColorTranslated[]            = { _T("Blue"), _T("Green"), _T("Pink"), _T("Red"), _T("Violet"), 
	                                      _T("Orange"), _T("Yellow"), _T("Brown"), _T("Black") };

// Advanced
tstring ILoadAnimationTranslated[]    = { _T("Off"), _T("On") };
tstring IEffectSingTranslated[]       = { _T("Off"), _T("On") };
tstring IScreenFadeTranslated[]       = { _T("Off"), _T("On") };
tstring IAskBeforeDelTranslated[]     = { _T("Off"), _T("On") };
tstring IOnSongClickTranslated[]      = { _T("Sing"), _T("Select Players"), _T("Open Menu") };

tstring ILineBonusTranslated[]        = { _T("Off"), _T("On") };
tstring IPartyPopupTranslated[]       = { _T("Off"), _T("On") };

tstring IJoypadTranslated[]           = { _T("Off"), _T("On") };
tstring IMouseTranslated[]            = { _T("Off"), _T("Hardware Cursor"), _T("Software Cursor") };

// Recording options
tstring IChannelPlayerTranslated[]    = { _T("Off"), _T("1"), _T("2"), _T("3"), _T("4"), _T("5"), _T("6") };
tstring IMicBoostTranslated[]         = { _T("Off"), _T("+6dB"), _T("+12dB"), _T("+18dB") };

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
				|| p.extension() != _T(".ini"))
				continue;

			tstring languageName = p.filename().stem().c_str();

			// insert the new language name to the set
			_langSet.insert(languageName);
		}
	}
	catch (filesystem_error)
	{
		sLog.Critical(_T("Language::LoadList"), _T("Could not access %s"), 
			LanguagesPath.native().c_str());
	}

	if (_langSet.empty())
		sLog.Critical(_T("Could not load any language file."));

	LanguageSet::const_iterator itr = _langSet.find(DEFAULT_LANGUAGE);
	if (itr == _langSet.end())
		sLog.Critical(_T("Default language (") DEFAULT_LANGUAGE _T(") does not exist."));
}

void Language::ChangeLanguage(const tstring& language)
{
	const TCHAR * sectionName = _T("Text");

	CSimpleIni ini(true);
	path iniPath(LanguagesPath);
	iniPath /= language;
	iniPath.replace_extension(_T(".ini"));

	SI_Error result = ini.LoadFile(iniPath.native().c_str());

	if (result != SI_OK)
		sLog.Critical(_T("Change language"), _T("Failed to set language to %s."), language.c_str());

	CSimpleIni::TNamesDepend keys;
	ini.GetAllKeys(sectionName, keys);
	for (CSimpleIni::TNamesDepend::const_iterator itr = keys.begin(); itr != keys.end(); ++itr)
		_langEntryMap[itr->pItem] = ini.GetValue(sectionName, itr->pItem);

	// Update language name for config
	sIni.LanguageName = language;

	// Translate & cache option values so we can avoid
	// looking them up on-demand.
	TranslateOptionValues();
}

void Language::AddConst(const tstring& id, const tstring& text)
{
	_langConstEntryMap[id] = text;
}

const tstring Language::Translate(const tstring& id)
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

const tstring Language::Translate(const TCHAR * id)
{
	tstring tmp = id;
	return Translate(tmp);
}

void Language::TranslateOptionValues()
{
	// Translate language names.
	_translatedLanguageNames.clear();
	for (LanguageSet::const_iterator itr = _langSet.begin(); itr != _langSet.end(); ++itr)
		_translatedLanguageNames[*itr] = Translate(_T("OPTION_VALUE_") + *itr);

	tstring switchOff = Translate(_T("OPTION_VALUE_OFF"));
	tstring switchOn  = Translate(_T("OPTION_VALUE_ON"));

	// Now translate the option values.
	IDifficultyTranslated[DifficultyType::Easy]           = Translate(_T("OPTION_VALUE_EASY"));
	IDifficultyTranslated[DifficultyType::Medium]         = Translate(_T("OPTION_VALUE_MEDIUM"));
	IDifficultyTranslated[DifficultyType::Hard]           = Translate(_T("OPTION_VALUE_HARD"));

	ITabsTranslated[Switch::Off]                          = switchOff;
	ITabsTranslated[Switch::On]                           = switchOn;

	ISortingTranslated[SortingType::Edition]              = Translate(_T("OPTION_VALUE_EDITION"));
	ISortingTranslated[SortingType::Genre]                = Translate(_T("OPTION_VALUE_GENRE"));
	ISortingTranslated[SortingType::Language]             = Translate(_T("OPTION_VALUE_LANGUAGE"));
	ISortingTranslated[SortingType::Folder]               = Translate(_T("OPTION_VALUE_FOLDER"));
	ISortingTranslated[SortingType::Title]                = Translate(_T("OPTION_VALUE_TITLE"));
	ISortingTranslated[SortingType::Artist]               = Translate(_T("OPTION_VALUE_ARTIST"));
	ISortingTranslated[SortingType::Artist2]              = Translate(_T("OPTION_VALUE_ARTIST2"));

	IDebugTranslated[Switch::Off]                         = switchOff;
	IDebugTranslated[Switch::On]                          = switchOn;

	IFullScreenTranslated[Switch::Off]                    = switchOff;
	IFullScreenTranslated[Switch::On]                     = switchOn;

	IVisualizerTranslated[VisualizerOption::Off]          = switchOff;
	IVisualizerTranslated[VisualizerOption::WhenNoVideo]  = Translate(_T("OPTION_VALUE_WHENNOVIDEO"));
	IVisualizerTranslated[VisualizerOption::On]           = switchOn;

	IBackgroundMusicTranslated[Switch::Off]               = switchOff;
	IBackgroundMusicTranslated[Switch::On]                = switchOn;

	ISingWindowTranslated[SingWindowType::Small]          = Translate(_T("OPTION_VALUE_SMALL"));
	ISingWindowTranslated[SingWindowType::Big]            = Translate(_T("OPTION_VALUE_BIG"));

	IOscilloscopeTranslated[Switch::Off]                  = switchOff;
	IOscilloscopeTranslated[Switch::On]                   = switchOn;

	ISpectrumTranslated[Switch::Off]                      = switchOff;
	ISpectrumTranslated[Switch::On]                       = switchOn;

	ISpectrographTranslated[Switch::Off]                  = switchOff;
	ISpectrographTranslated[Switch::On]                   = switchOn;

	IMovieSizeTranslated[0]                               = Translate(_T("OPTION_VALUE_HALF"));
	IMovieSizeTranslated[1]                               = Translate(_T("OPTION_VALUE_FULL_VID"));
	IMovieSizeTranslated[2]                               = Translate(_T("OPTION_VALUE_FULL_VID_BG"));

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

	ISyncToTranslated[SyncToType::Music]                  = Translate(_T("OPTION_VALUE_MUSIC"));
	ISyncToTranslated[SyncToType::Lyrics]                 = Translate(_T("OPTION_VALUE_LYRICS"));
	ISyncToTranslated[SyncToType::Off]                    = switchOff;

	ILyricsFontTranslated[LyricsFontType::Plain]          = Translate(_T("OPTION_VALUE_PLAIN"));
	ILyricsFontTranslated[LyricsFontType::OLine1]         = Translate(_T("OPTION_VALUE_OLINE1"));
	ILyricsFontTranslated[LyricsFontType::OLine2]         = Translate(_T("OPTION_VALUE_OLINE2"));

	ILyricsEffectTranslated[LyricsEffectType::Simple]     = Translate(_T("OPTION_VALUE_SIMPLE"));
	ILyricsEffectTranslated[LyricsEffectType::Zoom]       = Translate(_T("OPTION_VALUE_ZOOM"));
	ILyricsEffectTranslated[LyricsEffectType::Slide]      = Translate(_T("OPTION_VALUE_SLIDE"));
	ILyricsEffectTranslated[LyricsEffectType::Ball]       = Translate(_T("OPTION_VALUE_BALL"));
	ILyricsEffectTranslated[LyricsEffectType::Shift]      = Translate(_T("OPTION_VALUE_SHIFT"));

	INoteLinesTranslated[Switch::Off]                     = switchOff;
	INoteLinesTranslated[Switch::On]                      = switchOn;

	IColorTranslated[Color::Blue]                         = Translate(_T("OPTION_VALUE_BLUE"));
	IColorTranslated[Color::Green]                        = Translate(_T("OPTION_VALUE_GREEN"));
	IColorTranslated[Color::Pink]                         = Translate(_T("OPTION_VALUE_PINK"));
	IColorTranslated[Color::Red]                          = Translate(_T("OPTION_VALUE_RED"));
	IColorTranslated[Color::Violet]                       = Translate(_T("OPTION_VALUE_VIOLET"));
	IColorTranslated[Color::Orange]                       = Translate(_T("OPTION_VALUE_ORANGE"));
	IColorTranslated[Color::Yellow]                       = Translate(_T("OPTION_VALUE_YELLOW"));
	IColorTranslated[Color::Brown]                        = Translate(_T("OPTION_VALUE_BROWN"));
	IColorTranslated[Color::Black]                        = Translate(_T("OPTION_VALUE_BLACK"));

	// Advanced
	ILoadAnimationTranslated[Switch::Off]                 = switchOff;
	ILoadAnimationTranslated[Switch::On]                  = switchOn;

	IEffectSingTranslated[Switch::Off]                    = switchOff;
	IEffectSingTranslated[Switch::On]                     = switchOn;

	IScreenFadeTranslated[Switch::Off]                    = switchOff;
	IScreenFadeTranslated[Switch::On]                     = switchOn;

	IAskBeforeDelTranslated[Switch::Off]                  = switchOff;
	IAskBeforeDelTranslated[Switch::On]                   = switchOn;

	IOnSongClickTranslated[SongClickType::Sing]           = Translate(_T("OPTION_VALUE_SING"));
	IOnSongClickTranslated[SongClickType::SelectPlayers]  = Translate(_T("OPTION_VALUE_SELECT_PLAYERS"));
	IOnSongClickTranslated[SongClickType::OpenMenu]       = Translate(_T("OPTION_VALUE_OPEN_MENU"));

	ILineBonusTranslated[Switch::Off]                     = switchOff;
	ILineBonusTranslated[Switch::On]                      = switchOn;
 
	IPartyPopupTranslated[Switch::Off]                    = switchOff;
	IPartyPopupTranslated[Switch::On]                     = switchOn;

	IJoypadTranslated[Switch::Off]                        = switchOff;
	IJoypadTranslated[Switch::On]                         = switchOn;

	IMouseTranslated[0]                                   = switchOff;
	IMouseTranslated[1]                                   = Translate(_T("OPTION_VALUE_HARDWARE_CURSOR"));
	IMouseTranslated[2]                                   = Translate(_T("OPTION_VALUE_SOFTWARE_CURSOR"));

	IAudioOutputBufferSizeTranslated[0]                   = Translate(_T("OPTION_VALUE_AUTO"));
	IAudioInputBufferSizeTranslated[0]                    = Translate(_T("OPTION_VALUE_AUTO"));

	// Song Preview
	IPreviewVolumeTranslated[Switch::Off]                 = switchOff;
	IPreviewFadingTranslated[Switch::Off]                 = switchOff;
	IPreviewFadingTranslated[1]                           = _T("1 ") + Translate(_T("OPTION_VALUE_SEC"));
	IPreviewFadingTranslated[2]                           = _T("2 ") + Translate(_T("OPTION_VALUE_SECS"));
	IPreviewFadingTranslated[3]                           = _T("3 ") + Translate(_T("OPTION_VALUE_SECS"));
	IPreviewFadingTranslated[4]                           = _T("4 ") + Translate(_T("OPTION_VALUE_SECS"));
	IPreviewFadingTranslated[5]                           = _T("5 ") + Translate(_T("OPTION_VALUE_SECS"));

	// Recording options
	IChannelPlayerTranslated[Switch::Off]                 = switchOff;
	IMicBoostTranslated[Switch::Off]                      = switchOff;
}

Language::~Language()
{
}
