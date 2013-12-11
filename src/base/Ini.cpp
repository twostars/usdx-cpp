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
#include "Themes.h"
#include "Skins.h"

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

const ResolutionWH DefaultResolution(800, 600);
const TCHAR *      DefaultResolutionName = _T("800x600");

Ini::Ini()
{
	// Game
	Players = 0;
	Difficulty = DifficultyType::Easy;
	LanguageName = DEFAULT_LANGUAGE;
	Tabs = Switch::Off;
	TabsAtStartup = Tabs;
	Sorting = SortingType::Edition;
	Debug = Switch::Off;

	// Graphics
	Screens = 1;
	Resolution = DefaultResolution;
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
	Theme = NULL;
	Skin  = NULL;
	ThemeColor = Color::Blue;
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

	LoadProfileSettings(ini);
	LoadGameSettings(ini);
	LoadGraphicsSettings(ini);
	LoadSoundSettings(ini);
	LoadLyricsSettings(ini);
	LoadControllerSettings(ini);
	LoadAdvancedSettings(ini);

	LoadThemes(ini);
	LoadPaths(ini);
}

void Ini::LoadProfileSettings(CSimpleIni& ini)
{
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
}

void Ini::LoadGameSettings(CSimpleIni& ini)
{
	static const TCHAR * section = _T("Game");

	Players       = LOOKUP_ARRAY_INDEX(IPlayers,        section, _T("Players"), 0);
	Difficulty    = LOOKUP_ENUM_VALUE(DifficultyType,   section, _T("Difficulty"), DifficultyType::Easy);
	LanguageName  = ini.GetValue(section, _T("Language"), DEFAULT_LANGUAGE);

	Tabs          = LOOKUP_ENUM_VALUE(Switch,            section, _T("Tabs"), Switch::Off);
	TabsAtStartup = Tabs;

	Sorting       = LOOKUP_ENUM_VALUE(SortingType,       section, _T("Sorting"), SortingType::Edition);
	Debug         = LOOKUP_ENUM_VALUE(Switch,            section, _T("Debug"), Switch::Off);
}

void Ini::LoadGraphicsSettings(CSimpleIni& ini)
{
	static const TCHAR * section = _T("Graphics");

	Screens       = LOOKUP_ARRAY_INDEX(IScreens,        section, _T("Screens"), 0 /* 1 */);
	FullScreen    = LOOKUP_ENUM_VALUE(Switch,           section, _T("FullScreen"), Params.ScreenMode == scmWindowed ? Switch::Off : Switch::On);
	Depth         = LOOKUP_ARRAY_INDEX(IDepth,          section, _T("Depth"), 1 /* 32-bit */);

	// TextureSize (aka CachedCoverSize)
	TextureSize   = LOOKUP_ARRAY_INDEX(ITextureSize,    section, _T("TextureSize"), 0);
	SingWindow    = LOOKUP_ENUM_VALUE(SingWindowType,   section, _T("SingWindow"), SingWindowType::Big);
	Oscilloscope  = LOOKUP_ENUM_VALUE(Switch,           section, _T("Oscilloscope"), Switch::Off);
	Spectrum      = LOOKUP_ENUM_VALUE(Switch,           section, _T("Spectrum"), Switch::Off);
	Spectrograph  = LOOKUP_ENUM_VALUE(Switch,           section, _T("Spectrograph"), Switch::Off);
	MovieSize     = LOOKUP_ARRAY_INDEX(IMovieSize,      section, _T("MovieSize"), 2);
	VideoPreview  = LOOKUP_ENUM_VALUE(Switch,           section, _T("VideoPreview"), Switch::On);
	VideoEnabled  = LOOKUP_ENUM_VALUE(Switch,           section, _T("VideoEnabled"), Switch::On);
	VisualizerOption
                  = LOOKUP_ENUM_VALUE(VisualizerOption, section, _T("Visualization"), VisualizerOption::Off);

	LoadScreenModes(ini);

	tstring resName = ini.GetValue(section, _T("Resolution"), DefaultResolutionName);

	ReverseResolutionMap::const_iterator itr = ResolutionNameMap.find(resName);
	if (itr != ResolutionNameMap.end())
	{
		Resolution = itr->second;
	}
	else // not found, use default resolution.
	{
		Resolution = DefaultResolution;
		sLog.Warn(_T("Video"), _T("Invalid resolution specified (%s)."),
			resName.c_str());

		// Don't use fullscreen in this case.
		FullScreen = Switch::Off;
	}

	sLog.Status(_T("Video"), _T("Using resolution: %u x %u."), 
		Resolution.first, Resolution.second);
}

void Ini::LoadScreenModes(CSimpleIni& ini)
{
	SDL_Rect ** modes = SDL_ListModes(0);

	// Clear existing resolutions
	LoadedResolutions.clear();
	ResolutionNameMap.clear();

	// Load resolutions for this display device
	if (modes != NULL)
	{
		for (int i = 0; modes[i] != 0; i++)
		{
			SDL_Rect * mode = modes[i];
			AddResolution(mode->w, mode->h);
			SDL_free(modes[i]);
		}
		SDL_free(modes);
	}

	// No resolutions found, use default list.
	if (LoadedResolutions.empty())
	{
		AddResolution(640, 480);
		AddResolution(800, 600);
		AddResolution(1024, 768);
		AddResolution(1152, 666);
		AddResolution(1152, 864);
		AddResolution(1280, 800);
		AddResolution(1280, 960);
		AddResolution(1280, 1024);
		AddResolution(1366, 768);
		AddResolution(1400, 1050);
		AddResolution(1440, 900);
		AddResolution(1600, 900);
		AddResolution(1600, 1200);
		AddResolution(1680, 1050);
		AddResolution(1920, 1080);
		AddResolution(1920, 1200);
		AddResolution(2048, 1152);
		AddResolution(2560, 1600);
	}

	for (ResolutionMap::iterator itr = LoadedResolutions.begin(); 
		itr != LoadedResolutions.end();)
	{
		ResolutionMap::iterator itr2 = itr++;

		ResolutionWH resolution = itr2->first;
		tstring      resolutionName = itr2->second;

		// Handle width adjustment for multiple displays setup horizontally.
		if (Screens != 0)
		{
			// Remove old entry
			LoadedResolutions.erase(resolution);
			ResolutionNameMap.erase(resolutionName);

			// Adjust width (assume all screens are laid out horizontally)...
			resolution.first /= (Screens + 1);

			// Re-add new resolution
			AddResolution(resolution.first, resolution.second);
		}

		sLog.Status(_T("Video"), _T("Loaded resolution: %d x %d"), resolution.first, resolution.second);
	}

	sLog.Status(_T("Video"), _T("Loaded resolutions: %u"), LoadedResolutions.size());
}

void Ini::AddResolution(int width, int height)
{
	ResolutionWH resolution(width, height);
	TCHAR buffer[32];

	_sntprintf(buffer, 32, _T("%ux%u"), width, height);

	LoadedResolutions.insert(std::make_pair(resolution, buffer));
	ResolutionNameMap.insert(std::make_pair(buffer, resolution));
}

void Ini::LoadSoundSettings(CSimpleIni& ini)
{
	static const TCHAR * section = _T("Sound");

	ClickAssist   = LOOKUP_ENUM_VALUE(Switch,            section, _T("ClickAssist"), Switch::Off);
	BeatClick     = LOOKUP_ENUM_VALUE(Switch,            section, _T("BeatClick"), Switch::Off);
	SavePlayback  = LOOKUP_ENUM_VALUE(Switch,            section, _T("SavePlayback"), Switch::Off);
	PreviewVolume = LOOKUP_ARRAY_INDEX(IPreviewVolume,   section, _T("PreviewVolume"), 7 /* 100% */);
	PreviewFading = LOOKUP_ARRAY_INDEX(IPreviewFading,   section, _T("PreviewFading"), 3 /* 3 sec */);
	BackgroundMusic
	              = LOOKUP_ENUM_VALUE(Switch,            section, _T("BackgroundMusic"), Switch::On);
	AudioOutputBufferSizeIndex
	              = LOOKUP_ARRAY_INDEX(IAudioOutputBufferSize, section, _T("AudioOutputBufferSize"), 0);
	VoicePassthrough 
	         	  = LOOKUP_ENUM_VALUE(Switch,            section, _T("VoicePassthrough"), Switch::Off);
}

void Ini::LoadLyricsSettings(CSimpleIni& ini)
{
	static const TCHAR * section = _T("Lyrics");

	LyricsFont    = LOOKUP_ENUM_VALUE(LyricsFontType,    section, _T("LyricsFont"), LyricsFontType::Plain);
	LyricsEffect  = LOOKUP_ENUM_VALUE(LyricsEffectType,  section, _T("LyricsEffect"), LyricsEffectType::Shift);
	NoteLines     = LOOKUP_ENUM_VALUE(Switch,            section, _T("NoteLines"), Switch::On);

	DefaultEncoding
	              = LOOKUP_ENUM_VALUE(Encoding,          section, _T("Encoding"), Encoding::Auto);
}

void Ini::LoadAdvancedSettings(CSimpleIni& ini)
{
	static const TCHAR * section = _T("Advanced");

	LoadAnimation = LOOKUP_ENUM_VALUE(Switch,            section, _T("LoadAnimation"), Switch::On);
	ScreenFade    = LOOKUP_ENUM_VALUE(Switch,            section, _T("ScreenFade"), Switch::On);
	AskBeforeDel
	              = LOOKUP_ENUM_VALUE(Switch,            section, _T("AskbeforeDel"), Switch::On);
	OnSongClick   = LOOKUP_ENUM_VALUE(SongClickType,     section, _T("OnSongClick"), SongClickType::Sing);
	LineBonus     = LOOKUP_ENUM_VALUE(Switch,            section, _T("LineBonus"), Switch::On);
	PartyPopup    = LOOKUP_ENUM_VALUE(Switch,            section, _T("PartyPopup"), Switch::On);
	SyncTo        = LOOKUP_ENUM_VALUE(SyncToType,        section, _T("SyncTo"), SyncToType::Music);
}

void Ini::LoadControllerSettings(CSimpleIni& ini)
{
	static const TCHAR * section = _T("Controller");

	Joypad        = LOOKUP_ENUM_VALUE(Switch,            section, _T("Joypad"), Params.Joypad ? Switch::On : Switch::Off);
	Mouse         = LOOKUP_ARRAY_INDEX(IMouse,           section, _T("Mouse"), Params.Joypad ? 0 /* off */ : 2 /* software cursor */);

	LoadInputDeviceConfig(ini);
}

void Ini::LoadInputDeviceConfig(CSimpleIni& ini)
{
	// TODO
}

void Ini::LoadThemes(CSimpleIni& ini)
{
	if (sThemes.GetThemeCount() == 0)
		return sLog.Critical(_T("No themes are loaded."));

	static const TCHAR * section = _T("Themes");

	// TODO: Clean this up. It would be preferable to only store the theme & skin names here.
	tstring themeName = ini.GetValue(section, _T("Theme"), _T("DELUXE"));

	Theme = sThemes.LookupThemeDefault(themeName, DEFAULT_THEME);
	if (Theme == NULL)
		return sLog.Critical(_T("Theme not found, and default theme does not exist."));

	if (Theme->DefaultSkin == NULL)
		return sLog.Critical(_T("Specified theme doesn't have a default skin."));

	// sSkins.OnThemeChange();
	
	tstring skinName = ini.GetValue(section, _T("Skin"), Theme->DefaultSkin->Name.c_str());
	Skin = sSkins.LookupSkin(skinName);
	if (Skin == NULL)
		Skin = Theme->DefaultSkin; // NOTE: Checked earlier

	ThemeColor = LOOKUP_ENUM_VALUE(Color, section, _T("Color"), Skin->DefaultColor);
}

void Ini::LoadPaths(CSimpleIni& ini)
{
	const CSimpleIni::TKeyVal * section = ini.GetSection(_T("Directories"));
	for (CSimpleIni::TKeyVal::const_iterator itr = section->begin(); itr != section->end(); ++itr)
		AddSongPath(boost::filesystem::path(itr->second));
}

void Ini::Save()
{
}

Ini::~Ini()
{
}
