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
#include "Ini.h"
#include "PathUtils.h"
#include "Log.h"
#include "CommandLine.h"
#include "Language.h"

#include "../lib/simpleini/SimpleIni.h"

initialiseSingleton(Ini);

extern CMDParams Params;

/*
 * Options
 */

const tstring IPlayers[]          = { _T("1"), _T("2"), _T("3"), _T("4"), _T("6") };
const int IPlayersVals[]          = {     1,       2,       3,       4,       6   };

const tstring IScreens[]          = { _T("1"), _T("2") };

const tstring IDepth[]            = { _T("16 bit"), _T("32 bit") };
const tstring IVisualizer[]       = { _T("Off"), _T("WhenNoVideo"), _T("On") };

const tstring ITextureSize[]      = { _T("64"), _T("128"), _T("256"), _T("512") };
const int ITextureSizeVals[]      = {     64,       128,       256,       512   };

const tstring IMovieSize[]        = { _T("Half"), _T("Full [Vid]"), _T("Full [BG+Vid]") };

const tstring IThreshold[]        = { _T("5%"), _T("10%"), _T("15%"), _T("20%") };
const float IThresholdVals[]      = { 0.05f, 0.10f,  0.15f,  0.20f };

const tstring ISyncTo[]           = { _T("Music"), _T("Lyrics"), _T("Off") };

const tstring 
	IAudioOutputBufferSize[]      = { _T("Auto"), _T("256"),  _T("512"),   _T("1024"),  _T("2048"), 
	                                  _T("4096"), _T("8192"), _T("16384"), _T("32768"), _T("65536") };
const int 
	IAudioOutputBufferSizeVals[]  = {    0,           256,        512,         1024,        2048,
	                                     4096,        8192,       16384,       32768,       65536 };

const tstring
	IAudioInputBufferSize[]       = { _T("Auto"), _T("256"),  _T("512"),   _T("1024"),  _T("2048"), 
                                      _T("4096"), _T("8192"), _T("16384"), _T("32768"), _T("65536") };
const int
	IAudioInputBufferSizeVals[]   = {    0,           256,        512,         1024,        2048,
	                                     4096,        8192,       16384,       32768,       65536 };

// Song Preview
const tstring IPreviewVolume[]    = { _T("Off"), _T("10%"), _T("20%"), _T("30%"), _T("40%"), _T("50%"), 
	                                             _T("60%"), _T("70%"), _T("80%"), _T("90%"), _T("100%") };
const float IPreviewVolumeVals[]  = {    0.0f,      0.10f,     0.20f,     0.30f,     0.40f,     0.50f,  
	                                                0.60f,     0.70f,     0.80f,     0.90f,     1.00f };

const tstring IPreviewFading[]    = { _T("Off"), _T("1 Sec"), _T("2 Secs"), _T("3 Secs"), _T("4 Secs"), _T("5 Secs") };
const int IPreviewFadingVals[]    = {     0,         1,           2,            3,            4,            5        };

// Advanced
const tstring IMouse[]            = { _T("Off"), _T("Hardware Cursor"), _T("Software Cursor") };

// Recording options
const tstring IChannelPlayer[]    = { _T("Off"), _T("1"), _T("2"), _T("3"), _T("4"), _T("5"), _T("6") };
const tstring IMicBoost[]         = { _T("Off"), _T("+6dB"), _T("+12dB"), _T("+18dB") };

std::vector<tstring> IResolution;

/*
 * Translated options
 */

// TODO: Replace
std::vector<tstring> ILanguageTranslated;

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
tstring IAskbeforeDelTranslated[]     = { _T("Off"), _T("On") };
tstring IOnSongClickTranslated[]      = { _T("Sing"), _T("Select Players"), _T("Open Menu") };

tstring ILineBonusTranslated[]        = { _T("Off"), _T("On") };
tstring IPartyPopupTranslated[]       = { _T("Off"), _T("On") };

tstring IJoypadTranslated[]           = { _T("Off"), _T("On") };
tstring IMouseTranslated[]            = { _T("Off"), _T("Hardware Cursor"), _T("Software Cursor") };

// Recording options
tstring IChannelPlayerTranslated[]    = { _T("Off"), _T("1"), _T("2"), _T("3"), _T("4"), _T("5"), _T("6") };
tstring IMicBoostTranslated[]         = { _T("Off"), _T("+6dB"), _T("+12dB"), _T("+18dB") };

Ini::Ini()
{
	// Game
	Players = 0;
	Difficulty = DifficultyType::Easy;
	Language = DEFAULT_LANGUAGE;
	Tabs = Switch::Off;
	TabsAtStartup = Tabs;
	Sorting = SortingType::Edition;
	Debug = Switch::Off;

	// Graphics
	Screens = 1;
	Resolution = 0;
	Depth = 32;
	VisualizerOption = VisualizerOption::Off;
	FullScreen = Switch::On;
	TextureSize = 256;
	SingWindow = SingWindowType::Big;
	Oscilloscope = Switch::Off;
	Spectrum = Switch::Off;
	Spectrograph = Switch::Off;
	MovieSize = 2;
	VideoPreview = Switch::On;
	VideoEnabled = Switch::On;

	// Sound
	MicBoost = 0;
	ClickAssist = Switch::Off;
	BeatClick = Switch::Off;
	SavePlayback = Switch::Off;
	ThresholdIndex = 1;
	AudioOutputBufferSizeIndex = 0;
	VoicePassthrough = Switch::Off;
	SyncTo = SyncToType::Music;

	// Song preview
	PreviewVolume = 7;
	PreviewFading = 3;

	// Lyrics
	LyricsFont = LyricsFontType::Plain;
	LyricsEffect = LyricsEffectType::Shift;
	NoteLines = Switch::On;

	// Themes
	Theme = 0;
	SkinNo = 0;
	Color = Color::Blue;
	BackgroundMusic = Switch::On;

	// Advanced
	LoadAnimation = Switch::On;
	EffectSing = Switch::On;
	ScreenFade = Switch::On;
	AskBeforeDel = Switch::On;
	OnSongClick = SongClickType::Sing;
	LineBonus = Switch::On;
	PartyPopup = Switch::On;

	// Controller
	Joypad = Params.Joypad ? Switch::On : Switch::Off;
	Mouse  = Params.Joypad ? 0 : 2;

	// Default encoding for texts (lyrics, song name, ...)
	DefaultEncoding = Encoding::Auto;
}

void Ini::Load()
{
	const TCHAR * iniPath = GetConfigFile();
	CSimpleIni ini(true);

	sLog.Status(_T("Ini::Load"), _T("Using config: %s"), iniPath);

	SI_Error result = ini.LoadFile(iniPath);
	if (result != SI_OK)
		return sLog.Debug(_T("Ini::Load"), _T("Failed to load config."));

	// Load profile names/templates
	for (int i = 0; i < MAX_PROFILE_NAMES; i++)
	{
		TCHAR key[8], defaultName[10];

		_sntprintf(key, 8, _T("P%d"), i + 1);
		_sntprintf(defaultName, 10, _T("Player%d"), i + 1);

		Name[i] = ini.GetValue(_T("Name"), key, defaultName);

		_sntprintf(key, 8, _T("Name%d"), i + 1);
		_sntprintf(defaultName, 10, _T("Name%d"), i + 1);

		NameTemplate[i] = ini.GetValue(_T("NameTemplate"), key, defaultName);
	}

	// Load team name templates
	for (int i = 0; i < MAX_TEAMS; i++)
	{
		TCHAR key[4], defaultName[10];

		_sntprintf(key, 4, _T("T%d"), i + 1);
		_sntprintf(defaultName, 10, _T("Team%d"), i + 1);

		NameTeam[i] =  ini.GetValue(_T("NameTeam"), key, defaultName);
	}

	Players       = LOOKUP_ARRAY_INDEX(IPlayers,    _T("Game"), _T("Players"), 0);
	Difficulty    = LOOKUP_ENUM_VALUE(DifficultyType, _T("Game"), _T("Difficulty"), DifficultyType::Easy);
	Language      = ini.GetValue(_T("Game"), _T("Language"), DEFAULT_LANGUAGE);

	Tabs          = LOOKUP_ENUM_VALUE(Switch,        _T("Game"), _T("Tabs"), Switch::Off);
	TabsAtStartup = Tabs;

	Sorting       = LOOKUP_ENUM_VALUE(SortingType,   _T("Game"), _T("Sorting"), SortingType::Edition);
	Debug         = LOOKUP_ENUM_VALUE(Switch,       _T("Game"), _T("Debug"), Switch::Off);

	// TODO
	// LoadScreenModes();

	// TextureSize (aka CachedCoverSize)
	TextureSize   = LOOKUP_ARRAY_INDEX(ITextureSize,  _T("Graphics"), _T("TextureSize"), 0);
	SingWindow    = LOOKUP_ENUM_VALUE(SingWindowType, _T("Graphics"), _T("SingWindow"), SingWindowType::Big);
	Oscilloscope  = LOOKUP_ENUM_VALUE(Switch, _T("Graphics"), _T("Oscilloscope"), Switch::Off);
	Spectrum      = LOOKUP_ENUM_VALUE(Switch, _T("Graphics"), _T("Spectrum"), Switch::Off);
	Spectrograph  = LOOKUP_ENUM_VALUE(Switch, _T("Graphics"), _T("Spectrograph"), Switch::Off);
	MovieSize     = LOOKUP_ARRAY_INDEX(IMovieSize, _T("Graphics"), _T("MovieSize"), 2);
	VideoPreview  = LOOKUP_ENUM_VALUE(Switch, _T("Graphics"), _T("VideoPreview"), Switch::On);
	VideoEnabled  = LOOKUP_ENUM_VALUE(Switch, _T("Graphics"), _T("VideoEnabled"), Switch::On);
	VisualizerOption
                  = LOOKUP_ENUM_VALUE(VisualizerOption, _T("Graphics"), _T("Visualization"), VisualizerOption::Off);

	ClickAssist   = LOOKUP_ENUM_VALUE(Switch, _T("Sound"), _T("ClickAssist"), Switch::Off);
	BeatClick     = LOOKUP_ENUM_VALUE(Switch, _T("Sound"), _T("BeatClick"), Switch::Off);
	SavePlayback  = LOOKUP_ENUM_VALUE(Switch, _T("Sound"), _T("SavePlayback"), Switch::Off);
	PreviewVolume = LOOKUP_ARRAY_INDEX(IPreviewVolume, _T("Sound"), _T("PreviewVolume"), 7 /* 100% */);
	PreviewFading = LOOKUP_ARRAY_INDEX(IPreviewFading, _T("Sound"), _T("PreviewFading"), 3 /* 3 sec */);
	BackgroundMusic
	              = LOOKUP_ENUM_VALUE(Switch, _T("Sound"), _T("BackgroundMusic"), Switch::On);
	AudioOutputBufferSizeIndex
	              = LOOKUP_ARRAY_INDEX(IAudioOutputBufferSize, _T("Sound"), _T("AudioOutputBufferSize"), 0);
	VoicePassthrough 
	         	  = LOOKUP_ENUM_VALUE(Switch, _T("Sound"), _T("VoicePassthrough"), Switch::Off);

	LyricsFont    = LOOKUP_ENUM_VALUE(LyricsFontType, _T("Lyrics"), _T("LyricsFont"), LyricsFontType::Plain);
	LyricsEffect  = LOOKUP_ENUM_VALUE(LyricsEffectType, _T("Lyrics"), _T("LyricsEffect"), LyricsEffectType::Shift);
	NoteLines     = LOOKUP_ENUM_VALUE(Switch, _T("Lyrics"), _T("NoteLines"), Switch::On);

	DefaultEncoding
	              = LOOKUP_ENUM_VALUE(Encoding, _T("Lyrics"), _T("Encoding"), Encoding::Auto);

	// TODO
	// LoadThemes();
	// LoadInputDeviceCfg(ini);

	LoadAnimation = LOOKUP_ENUM_VALUE(Switch, _T("Advanced"), _T("LoadAnimation"), Switch::On);
	ScreenFade    = LOOKUP_ENUM_VALUE(Switch, _T("Advanced"), _T("ScreenFade"), Switch::On);
	AskBeforeDel
	              = LOOKUP_ENUM_VALUE(Switch, _T("Advanced"), _T("AskbeforeDel"), Switch::On);
	OnSongClick   = LOOKUP_ENUM_VALUE(SongClickType, _T("Advanced"), _T("OnSongClick"), SongClickType::Sing);
	LineBonus     = LOOKUP_ENUM_VALUE(Switch, _T("Advanced"), _T("LineBonus"), Switch::On);
	PartyPopup    = LOOKUP_ENUM_VALUE(Switch, _T("Advanced"), _T("PartyPopup"), Switch::On);
	SyncTo        = LOOKUP_ENUM_VALUE(SyncToType, _T("Advanced"), _T("SyncTo"), SyncToType::Music);

	Joypad        = LOOKUP_ENUM_VALUE(Switch, _T("Controller"), _T("Joypad"), Params.Joypad ? Switch::On : Switch::Off);
	Mouse         = LOOKUP_ARRAY_INDEX(IMouse, _T("Controller"), _T("Mouse"), Params.Joypad ? 0 /* off */ : 2 /* software cursor */);

	// TODO
	// LoadPaths();
	// TranslateOptionValues();
}

void Ini::Save()
{
}

Ini::~Ini()
{
}
