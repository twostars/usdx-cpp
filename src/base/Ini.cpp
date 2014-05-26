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
extern PathSet SongPaths;

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
	: ini(true)
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

	sLog.Status(_T("Ini::Load"), _T("Using config: %s"), iniPath);

	SI_Error result = ini.LoadFile(iniPath);
	if (result != SI_OK)
		return sLog.Debug(_T("Ini::Load"), _T("Failed to load config."));

	LoadProfileSettings();
	LoadGameSettings();
	LoadGraphicsSettings();
	LoadSoundSettings();
	LoadLyricsSettings();
	LoadAdvancedSettings();
	LoadControllerSettings();
	LoadInputDeviceConfig();

	LoadThemes();
	LoadPaths();

	sLog.Status(_T("Ini::Load"), _T("Loaded config."));
}

void Ini::Save()
{
	SaveProfileSettings();
	SaveGameSettings();
	SaveGraphicsSettings();
	SaveSoundSettings();
	SaveLyricsSettings();
	SaveAdvancedSettings();
	SaveControllerSettings();
	SaveInputDeviceConfig();

	SaveThemes();
	SavePaths();

	SaveToFile();
}

void Ini::SaveToFile()
{
	const TCHAR * iniFile = GetConfigFile();
	SI_Error result = ini.SaveFile(iniFile);
	if (result != SI_OK)
		sLog.Error(_T("Ini::SaveToFile"), _T("Failed to save config to file: %s"), iniFile);
	else
		sLog.Status(_T("Ini::SaveToFile"), _T("Saved config to file: %s"), iniFile);
}

void Ini::LoadProfileSettings()
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

void Ini::SaveProfileSettings()
{
	// Save profile names/templates
	for (int i = 0; i < MAX_PROFILE_NAMES; i++)
	{
		TCHAR key[8];

		_sntprintf(key, 8, _T("P%d"), i + 1);
		ini.SetValue(_T("Name"), key, Name[i].c_str());

		_sntprintf(key, 8, _T("Name%d"), i + 1);
		ini.SetValue(_T("NameTemplate"), key, NameTemplate[i].c_str());
	}

	// Save team name templates
	for (int i = 0; i < MAX_TEAMS; i++)
	{
		TCHAR key[4];

		_sntprintf(key, 4, _T("T%d"), i + 1);
		ini.SetValue(_T("NameTeam"), key, NameTeam[i].c_str());
	}
}

void Ini::LoadGameSettings()
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

void Ini::SaveGameSettings()
{
	static const TCHAR * section = _T("Game");

	ini.SetValue(section, _T("Players"), IPlayers[Players].c_str());
	ini.SetValue(section, _T("Difficulty"), Enum2String(Difficulty).c_str());
	ini.SetValue(section, _T("Language"), LanguageName.c_str());

	SAVE_ENUM_VALUE(section, _T("Tabs"), Tabs);
	SAVE_ENUM_VALUE(section, _T("Sorting"), Sorting);
	SAVE_ENUM_VALUE(section, _T("Debug"), Debug);
}

void Ini::LoadGraphicsSettings()
{
	static const TCHAR * section = _T("Graphics");

	Screens       = LOOKUP_ARRAY_INDEX(IScreens,        section, _T("Screens"), 0 /* 1 */);
	FullScreen    = LOOKUP_ENUM_VALUE(Switch,           section, _T("FullScreen"), Switch::On);
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

	LoadScreenModes();

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

void Ini::SaveGraphicsSettings()
{
	static const TCHAR * section = _T("Graphics");

	ini.SetValue(section, _T("Screens"), IScreens[Screens].c_str());
	SAVE_ENUM_VALUE(section, _T("FullScreen"), FullScreen);
	ini.SetValue(section, _T("Depth"), IDepth[Depth].c_str());

	// TextureSize (aka CachedCoverSize)
	ini.SetValue(section, _T("TextureSize"), ITextureSize[TextureSize].c_str());
	SAVE_ENUM_VALUE(section, _T("SingWindow"), SingWindow);
	SAVE_ENUM_VALUE(section, _T("Oscilloscope"), Oscilloscope);
	SAVE_ENUM_VALUE(section, _T("Spectrum"), Spectrum);
	SAVE_ENUM_VALUE(section, _T("Spectrograph"), Spectrograph);
	ini.SetValue(section, _T("MovieSize"), IMovieSize[MovieSize].c_str());
	SAVE_ENUM_VALUE(section, _T("VideoPreview"), VideoPreview);
	SAVE_ENUM_VALUE(section, _T("VideoEnabled"), VideoEnabled);

	SAVE_ENUM_VALUE(section, _T("Visualization"), VisualizerOption);

	ResolutionMap::iterator itr = LoadedResolutions.find(Resolution);
	tstring resName;
	if (itr != LoadedResolutions.end())
		resName = itr->second;
	else
		resName = DefaultResolutionName;

	ini.SetValue(section, _T("Resolution"), resName.c_str());
}

void Ini::LoadScreenModes()
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

	sLog.Status(_T("Video"), _T("Loaded resolutions: %u"), LoadedResolutions.size());
}

void Ini::AddResolution(int width, int height)
{
	ResolutionWH resolution(width, height);
	TCHAR buffer[32];

	_sntprintf(buffer, 32, _T("%ux%u"), width, height);

	LoadedResolutions.insert(std::make_pair(resolution, buffer));
	ResolutionNameMap.insert(std::make_pair(buffer, resolution));

	sLog.Status(_T("Video"), _T("Loaded resolution: %d x %d"), width, height);
}

void Ini::LoadSoundSettings()
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

void Ini::SaveSoundSettings()
{
	static const TCHAR * section = _T("Sound");

	SAVE_ENUM_VALUE(section, _T("ClickAssist"), ClickAssist);
	SAVE_ENUM_VALUE(section, _T("BeatClick"), BeatClick);
	SAVE_ENUM_VALUE(section, _T("SavePlayback"), SavePlayback);
	ini.SetValue(section, _T("PreviewVolume"), IPreviewVolume[PreviewVolume].c_str());
	ini.SetValue(section, _T("PreviewFading"), IPreviewFading[PreviewFading].c_str());
	SAVE_ENUM_VALUE(section, _T("BackgroundMusic"), BackgroundMusic);
	ini.SetValue(section, _T("AudioOutputBufferSize"), IAudioOutputBufferSize[AudioOutputBufferSizeIndex].c_str());
	SAVE_ENUM_VALUE(section, _T("VoicePassthrough"), VoicePassthrough);
}

void Ini::LoadLyricsSettings()
{
	static const TCHAR * section = _T("Lyrics");

	LyricsFont    = LOOKUP_ENUM_VALUE(LyricsFontType,    section, _T("LyricsFont"), LyricsFontType::Plain);
	LyricsEffect  = LOOKUP_ENUM_VALUE(LyricsEffectType,  section, _T("LyricsEffect"), LyricsEffectType::Shift);
	NoteLines     = LOOKUP_ENUM_VALUE(Switch,            section, _T("NoteLines"), Switch::On);

	DefaultEncoding
	              = LOOKUP_ENUM_VALUE(Encoding,          section, _T("Encoding"), Encoding::Auto);
}

void Ini::SaveLyricsSettings()
{
	static const TCHAR * section = _T("Lyrics");

	SAVE_ENUM_VALUE(section, _T("LyricsFont"), LyricsFont);
	SAVE_ENUM_VALUE(section, _T("LyricsEffect"), LyricsEffect);
	SAVE_ENUM_VALUE(section, _T("NoteLines"), NoteLines);
	SAVE_ENUM_VALUE(section, _T("DefaultEncoding"), DefaultEncoding);
}

void Ini::LoadAdvancedSettings()
{
	static const TCHAR * section = _T("Advanced");

	LoadAnimation = LOOKUP_ENUM_VALUE(Switch,            section, _T("LoadAnimation"), Switch::On);
	ScreenFade    = LOOKUP_ENUM_VALUE(Switch,            section, _T("ScreenFade"), Switch::On);
	AskBeforeDel
	              = LOOKUP_ENUM_VALUE(Switch,            section, _T("AskBeforeDel"), Switch::On);
	OnSongClick   = LOOKUP_ENUM_VALUE(SongClickType,     section, _T("OnSongClick"), SongClickType::Sing);
	LineBonus     = LOOKUP_ENUM_VALUE(Switch,            section, _T("LineBonus"), Switch::On);
	PartyPopup    = LOOKUP_ENUM_VALUE(Switch,            section, _T("PartyPopup"), Switch::On);
	SyncTo        = LOOKUP_ENUM_VALUE(SyncToType,        section, _T("SyncTo"), SyncToType::Music);
}

void Ini::SaveAdvancedSettings()
{
	static const TCHAR * section = _T("Advanced");

	SAVE_ENUM_VALUE(section, _T("LoadAnimation"), LoadAnimation);
	SAVE_ENUM_VALUE(section, _T("ScreenFade"), ScreenFade);
	SAVE_ENUM_VALUE(section, _T("AskBeforeDel"), AskBeforeDel);

	SAVE_ENUM_VALUE(section, _T("OnSongClick"), OnSongClick);
	SAVE_ENUM_VALUE(section, _T("LineBonus"), LineBonus);
	SAVE_ENUM_VALUE(section, _T("PartyPopup"), PartyPopup);
	SAVE_ENUM_VALUE(section, _T("SyncTo"), SyncTo);
}

void Ini::LoadControllerSettings()
{
	static const TCHAR * section = _T("Controller");

	Joypad        = LOOKUP_ENUM_VALUE(Switch,            section, _T("Joypad"), Params.Joypad ? Switch::On : Switch::Off);
	Mouse         = LOOKUP_ARRAY_INDEX(IMouse,           section, _T("Mouse"), Params.Joypad ? 0 /* off */ : 2 /* software cursor */);
}

void Ini::SaveControllerSettings()
{
	static const TCHAR * section = _T("Controller");

	SAVE_ENUM_VALUE(section, _T("Joypad"), Joypad);
	ini.SetValue(section, _T("Mouse"), IMouse[Mouse].c_str());
}

void Ini::LoadInputDeviceConfig()
{
	static const tstring deviceNameKey(_T("DeviceName"));
	const TCHAR * section = _T("Record");

	// Clear the device map
	InputDevices.clear();

	const CSimpleIni::TKeyVal * sectionKeys = ini.GetSection(section);
	if (sectionKeys == NULL)
		return;

	for (CSimpleIni::TKeyVal::const_iterator itr = sectionKeys->begin(); itr != sectionKeys->end(); ++itr)
	{
		tstring key = itr->first.pItem;
		if (key.length() < deviceNameKey.length()
			|| key.substr(0, deviceNameKey.length()) != deviceNameKey)
			continue;

		TCHAR buff[20];
		int deviceNo;
		_stscanf(key.c_str(), _T("%10s[%d]"), buff, &deviceNo);

		InputDeviceConfig device;
		device.Name = itr->second;

		_sntprintf(buff, 20, _T("Input[%d]"), deviceNo);
		device.Input = ini.GetLongValue(section, buff, 0);

		_sntprintf(buff, 20, _T("Latency[%d]"), deviceNo);
		device.Latency = ini.GetLongValue(section, buff, LATENCY_AUTODETECT);

		for (int channelIndex = 0; ; channelIndex++)
		{
			_sntprintf(buff, 20, _T("Channel%d[%d]"), channelIndex + 1, deviceNo);
			int channelPlayer = ini.GetLongValue(section, buff, -1);
			if (channelPlayer < 0)
				break;

			device.ChannelMap[channelIndex] = channelPlayer;
		}

		InputDevices.insert(std::make_pair(deviceNo, device));
	}
}

void Ini::SaveInputDeviceConfig()
{
	const TCHAR * section = _T("Record");

	// Remove entire recording device section.
	// As we still have the config file loaded in memory,
	// we still have old device config which may need to be removed.
	// Removing the entire section & replacing it ensures there's no lingering data.
	ini.Delete(section, NULL, true);

	for (InputDeviceConfigMap::const_iterator itr = InputDevices.begin();
		itr != InputDevices.end();
		++itr)
	{
		TCHAR buff[20];
		const InputDeviceConfig& device = itr->second;
		int deviceNo = itr->first;

		_sntprintf(buff, 20, _T("DeviceName[%d]"), deviceNo);
		ini.SetValue(section, buff, device.Name.c_str());

		_sntprintf(buff, 20, _T("Input[%d]"), deviceNo);
		ini.SetLongValue(section, buff, device.Input);

		_sntprintf(buff, 20, _T("Latency[%d]"), deviceNo);
		ini.SetLongValue(section, buff, device.Latency);

		for (ChannelToPlayerMap::const_iterator channelItr = device.ChannelMap.begin();
			channelItr != device.ChannelMap.end();
			++channelItr)
		{
			_sntprintf(buff, 20, _T("Channel%d[%d]"), channelItr->first, deviceNo);
			ini.SetLongValue(section, buff, channelItr->second);
		}
	}
}

void Ini::LoadThemes()
{
	if (sThemes.GetThemeCount() == 0)
		return sLog.Critical(_T("No themes are loaded."));

	static const TCHAR * section = _T("Themes");

	// TODO: Clean this up. It would be preferable to only store the theme & skin names here.
	tstring themeName = ini.GetValue(section, _T("Theme"), DEFAULT_THEME);

	Theme = sThemes.LookupThemeDefault(themeName, DEFAULT_THEME);
	if (Theme == NULL)
		return sLog.Critical(_T("Theme not found, and default theme does not exist."));

	if (Theme->DefaultSkin == NULL)
		return sLog.Critical(_T("Specified theme doesn't have a default skin."));

	tstring skinName = ini.GetValue(section, _T("Skin"), Theme->DefaultSkin->Name.c_str());
	Skin = sSkins.LookupSkinForTheme(skinName, Theme->Name);
	if (Skin == NULL)
		Skin = Theme->DefaultSkin; // NOTE: Checked earlier

	ThemeColor = LOOKUP_ENUM_VALUE(Color, section, _T("Color"), Skin->DefaultColor);
}

void Ini::SaveThemes()
{
	static const TCHAR * section = _T("Themes");

	// TODO: Clean this up. It would be preferable to only store the theme & skin names here.
	ini.SetValue(section, _T("Theme"), Theme->Name.c_str());
	ini.SetValue(section, _T("Skin"), Theme->DefaultSkin->Name.c_str());
	SAVE_ENUM_VALUE(section, _T("Color"), Theme->DefaultSkin->DefaultColor);
}

void Ini::LoadPaths()
{
	const TCHAR * section = _T("Directories");
	const CSimpleIni::TKeyVal * sectionKeys = ini.GetSection(section);
	if (sectionKeys == NULL)
		return;

	for (CSimpleIni::TKeyVal::const_iterator itr = sectionKeys->begin(); itr != sectionKeys->end(); ++itr)
		AddSongPath(boost::filesystem::path(itr->second));
}

void Ini::SavePaths()
{
	const TCHAR * section = _T("Directories");

	// Remove entire directories section.
	// As we still have the config file loaded in memory,
	// we still have old directories which may need to be removed.
	// Removing the entire section & replacing it ensures there's no lingering data.
	ini.Delete(section, NULL, true);

	int n = 1;
	for (PathSet::const_iterator itr = SongPaths.begin(); itr != SongPaths.end(); ++itr)
	{
		TCHAR buff[20];
		_sntprintf(buff, 20, _T("SongDir%d"), n++);
		ini.SetValue(section, buff, itr->native().c_str());
	}
}

Ini::~Ini()
{
}
