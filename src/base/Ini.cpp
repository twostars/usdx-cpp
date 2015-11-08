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

const std::string IPlayers[]          = { "1", "2", "3", "4", "6" };
const int IPlayersVals[]          = {     1,       2,       3,       4,       6   };

const std::string IScreens[]          = { "1", "2" };

const std::string IDepth[]            = { "16 bit", "32 bit" };
const std::string IVisualizer[]       = { "Off", "WhenNoVideo", "On" };

const std::string ITextureSize[]      = { "64", "128", "256", "512" };
const int ITextureSizeVals[]      = {     64,       128,       256,       512   };

const std::string IMovieSize[]        = { "Half", "Full [Vid]", "Full [BG+Vid]" };

const std::string IThreshold[]        = { "5%", "10%", "15%", "20%" };
const float IThresholdVals[]      = { 0.05f, 0.10f,  0.15f,  0.20f };

const std::string ISyncTo[]           = { "Music", "Lyrics", "Off" };

const std::string 
	IAudioOutputBufferSize[]      = { "Auto", "256",  "512",   "1024",  "2048", 
	                                  "4096", "8192", "16384", "32768", "65536" };
const int 
	IAudioOutputBufferSizeVals[]  = {    0,           256,        512,         1024,        2048,
	                                     4096,        8192,       16384,       32768,       65536 };

const std::string
	IAudioInputBufferSize[]       = { "Auto", "256",  "512",   "1024",  "2048", 
                                      "4096", "8192", "16384", "32768", "65536" };
const int
	IAudioInputBufferSizeVals[]   = {    0,           256,        512,         1024,        2048,
	                                     4096,        8192,       16384,       32768,       65536 };

// Song Preview
const std::string IPreviewVolume[]    = { "Off", "10%", "20%", "30%", "40%", "50%", 
	                                             "60%", "70%", "80%", "90%", "100%" };
const float IPreviewVolumeVals[]  = {    0.0f,      0.10f,     0.20f,     0.30f,     0.40f,     0.50f,  
	                                                0.60f,     0.70f,     0.80f,     0.90f,     1.00f };

const std::string IPreviewFading[]    = { "Off", "1 Sec", "2 Secs", "3 Secs", "4 Secs", "5 Secs" };
const int IPreviewFadingVals[]    = {     0,         1,           2,            3,            4,            5        };

// Advanced
const std::string IMouse[]            = { "Off", "Hardware Cursor", "Software Cursor" };

// Recording options
const std::string IChannelPlayer[]    = { "Off", "1", "2", "3", "4", "5", "6" };
const std::string IMicBoost[]         = { "Off", "+6dB", "+12dB", "+18dB" };

const ResolutionWH DefaultResolution(800, 600);
const char *      DefaultResolutionName = "800x600";

Ini::Ini()
	: ini(true)
{
	// Game
	Players = 0;
	Difficulty = DifficultyType::Easy;
	Language = 0;
	LanguageName = DEFAULT_LANGUAGE;
	Tabs = Switch::Off;
	TabsAtStartup = Tabs;
	Sorting = SortingType::Edition;
	Debug = Switch::Off;

	// Graphics
	Screens = 1;
	Resolution = DefaultResolution;
	SelectedResolution = 0;
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
	const std::string iniPath = GetConfigFile();

	sLog.Status("Ini::Load", "Using config: %s", iniPath.c_str());

	SI_Error result = ini.LoadFile(iniPath.c_str());
	if (result != SI_OK)
		return sLog.Debug("Ini::Load", "Failed to load config.");

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

	sLog.Status("Ini::Load", "Loaded config.");
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
	const std::string iniFile = GetConfigFile();
	SI_Error result = ini.SaveFile(iniFile.c_str());
	if (result != SI_OK)
		sLog.Error("Ini::SaveToFile", "Failed to save config to file: %s", iniFile.c_str());
	else
		sLog.Status("Ini::SaveToFile", "Saved config to file: %s", iniFile.c_str());
}

void Ini::LoadProfileSettings()
{
	// Load profile names/templates
	for (int i = 0; i < MAX_PROFILE_NAMES; i++)
	{
		char key[8], defaultName[10];

		snprintf(key, 8, "P%d", i + 1);
		snprintf(defaultName, 10, "Player%d", i + 1);

		Name[i] = ini.GetValue("Name", key, defaultName);

		snprintf(key, 8, "Name%d", i + 1);
		snprintf(defaultName, 10, "Name%d", i + 1);

		NameTemplate[i] = ini.GetValue("NameTemplate", key, defaultName);
	}

	// Load team name templates
	for (int i = 0; i < MAX_TEAMS; i++)
	{
		char key[4], defaultName[10];

		snprintf(key, 4, "T%d", i + 1);
		snprintf(defaultName, 10, "Team%d", i + 1);

		NameTeam[i] =  ini.GetValue("NameTeam", key, defaultName);
	}
}

void Ini::SaveProfileSettings()
{
	// Save profile names/templates
	for (int i = 0; i < MAX_PROFILE_NAMES; i++)
	{
		char key[8];

		snprintf(key, 8, "P%d", i + 1);
		ini.SetValue("Name", key, Name[i].c_str());

		snprintf(key, 8, "Name%d", i + 1);
		ini.SetValue("NameTemplate", key, NameTemplate[i].c_str());
	}

	// Save team name templates
	for (int i = 0; i < MAX_TEAMS; i++)
	{
		char key[4];

		snprintf(key, 4, "T%d", i + 1);
		ini.SetValue("NameTeam", key, NameTeam[i].c_str());
	}
}

void Ini::LoadGameSettings()
{
	static const char * section = "Game";

	Players       = LOOKUP_ARRAY_INDEX(IPlayers,        section, "Players", 0);
	Difficulty    = LOOKUP_ENUM_VALUE(DifficultyType,   section, "Difficulty", DifficultyType::Easy);
	LanguageName  = ini.GetValue(section, "Language", DEFAULT_LANGUAGE);
	Language      = sLanguage.GetLanguageID(LanguageName);

	Tabs          = LOOKUP_ENUM_VALUE(Switch,            section, "Tabs", Switch::Off);
	TabsAtStartup = Tabs;

	Sorting       = LOOKUP_ENUM_VALUE(SortingType,       section, "Sorting", SortingType::Edition);
	Debug         = LOOKUP_ENUM_VALUE(Switch,            section, "Debug", Switch::Off);
}

void Ini::SaveGameSettings()
{
	static const char * section = "Game";

	// Set the language name.
	LanguageName = sLanguage.GetLanguageByID(Language);

	ini.SetValue(section, "Players", IPlayers[Players].c_str());
	ini.SetValue(section, "Difficulty", Enum2String(Difficulty).c_str());
	ini.SetValue(section, "Language", LanguageName.c_str());

	SAVE_ENUM_VALUE(section, "Tabs", Tabs);
	SAVE_ENUM_VALUE(section, "Sorting", Sorting);
	SAVE_ENUM_VALUE(section, "Debug", Debug);
}

void Ini::LoadGraphicsSettings()
{
	static const char * section = "Graphics";

	Screens       = LOOKUP_ARRAY_INDEX(IScreens,        section, "Screens", 0 /* 1 */);
	FullScreen    = LOOKUP_ENUM_VALUE(Switch,           section, "FullScreen", Switch::On);
	Depth         = LOOKUP_ARRAY_INDEX(IDepth,          section, "Depth", 1 /* 32-bit */);

	// TextureSize (aka CachedCoverSize)
	TextureSize   = LOOKUP_ARRAY_INDEX(ITextureSize,    section, "TextureSize", 0);
	SingWindow    = LOOKUP_ENUM_VALUE(SingWindowType,   section, "SingWindow", SingWindowType::Big);
	Oscilloscope  = LOOKUP_ENUM_VALUE(Switch,           section, "Oscilloscope", Switch::Off);
	Spectrum      = LOOKUP_ENUM_VALUE(Switch,           section, "Spectrum", Switch::Off);
	Spectrograph  = LOOKUP_ENUM_VALUE(Switch,           section, "Spectrograph", Switch::Off);
	MovieSize     = LOOKUP_ARRAY_INDEX(IMovieSize,      section, "MovieSize", 2);
	VideoPreview  = LOOKUP_ENUM_VALUE(Switch,           section, "VideoPreview", Switch::On);
	VideoEnabled  = LOOKUP_ENUM_VALUE(Switch,           section, "VideoEnabled", Switch::On);
	VisualizerOption
                  = LOOKUP_ENUM_VALUE(VisualizerOption, section, "Visualization", VisualizerOption::Off);

	LoadScreenModes();

	std::string resName = ini.GetValue(section, "Resolution", DefaultResolutionName);

	ReverseResolutionMap::const_iterator itr = ResolutionNameMap.find(resName);
	if (itr != ResolutionNameMap.end())
	{
		Resolution = itr->second;
	}
	else // not found, use default resolution.
	{
		Resolution = DefaultResolution;
		sLog.Warn("Video", "Invalid resolution specified (%s).",
			resName.c_str());

		// Don't use fullscreen in this case.
		FullScreen = Switch::Off;
	}

	// Get the selected resolution's ID for option configuration/selection.
	SelectedResolution = GetResolutionID(resName);

	sLog.Status("Video", "Using resolution: %u x %u.", 
		Resolution.first, Resolution.second);
}

void Ini::SaveGraphicsSettings()
{
	static const char * section = "Graphics";

	ini.SetValue(section, "Screens", IScreens[Screens].c_str());
	SAVE_ENUM_VALUE(section, "FullScreen", FullScreen);
	ini.SetValue(section, "Depth", IDepth[Depth].c_str());

	// TextureSize (aka CachedCoverSize)
	ini.SetValue(section, "TextureSize", ITextureSize[TextureSize].c_str());
	SAVE_ENUM_VALUE(section, "SingWindow", SingWindow);
	SAVE_ENUM_VALUE(section, "Oscilloscope", Oscilloscope);
	SAVE_ENUM_VALUE(section, "Spectrum", Spectrum);
	SAVE_ENUM_VALUE(section, "Spectrograph", Spectrograph);
	ini.SetValue(section, "MovieSize", IMovieSize[MovieSize].c_str());
	SAVE_ENUM_VALUE(section, "VideoPreview", VideoPreview);
	SAVE_ENUM_VALUE(section, "VideoEnabled", VideoEnabled);

	SAVE_ENUM_VALUE(section, "Visualization", VisualizerOption);

	std::string selectedResolution = ResolutionNameList[SelectedResolution];
	ini.SetValue(section, "Resolution", selectedResolution.c_str());
}

void Ini::LoadScreenModes()
{
	SDL_Rect ** modes = SDL_ListModes(0);

	// Clear existing resolutions
	LoadedResolutions.clear();
	ResolutionNameMap.clear();
	ResolutionNameList.clear();

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

	// Load resolution list now so as to ensure it's in the correct order (lowest -> highest).
	for (ResolutionMap::iterator itr = LoadedResolutions.begin(); itr != LoadedResolutions.end(); ++itr)
		ResolutionNameList.push_back(itr->second);

	sLog.Status("Video", "Loaded resolutions: %u", LoadedResolutions.size());
}

void Ini::AddResolution(int width, int height)
{
	ResolutionWH resolution(width, height);
	char buffer[32];

	snprintf(buffer, 32, "%ux%u", width, height);

	LoadedResolutions.insert(std::make_pair(resolution, buffer));
	ResolutionNameMap.insert(std::make_pair(buffer, resolution));

	sLog.Status("Video", "Loaded resolution: %d x %d", width, height);
}

void Ini::LoadSoundSettings()
{
	static const char * section = "Sound";

	ClickAssist   = LOOKUP_ENUM_VALUE(Switch,            section, "ClickAssist", Switch::Off);
	BeatClick     = LOOKUP_ENUM_VALUE(Switch,            section, "BeatClick", Switch::Off);
	SavePlayback  = LOOKUP_ENUM_VALUE(Switch,            section, "SavePlayback", Switch::Off);
	PreviewVolume = LOOKUP_ARRAY_INDEX(IPreviewVolume,   section, "PreviewVolume", 7 /* 100% */);
	PreviewFading = LOOKUP_ARRAY_INDEX(IPreviewFading,   section, "PreviewFading", 3 /* 3 sec */);
	BackgroundMusic
	              = LOOKUP_ENUM_VALUE(Switch,            section, "BackgroundMusic", Switch::On);
	AudioOutputBufferSizeIndex
	              = LOOKUP_ARRAY_INDEX(IAudioOutputBufferSize, section, "AudioOutputBufferSize", 0);
	VoicePassthrough 
	         	  = LOOKUP_ENUM_VALUE(Switch,            section, "VoicePassthrough", Switch::Off);
}

void Ini::SaveSoundSettings()
{
	static const char * section = "Sound";

	SAVE_ENUM_VALUE(section, "ClickAssist", ClickAssist);
	SAVE_ENUM_VALUE(section, "BeatClick", BeatClick);
	SAVE_ENUM_VALUE(section, "SavePlayback", SavePlayback);
	ini.SetValue(section, "PreviewVolume", IPreviewVolume[PreviewVolume].c_str());
	ini.SetValue(section, "PreviewFading", IPreviewFading[PreviewFading].c_str());
	SAVE_ENUM_VALUE(section, "BackgroundMusic", BackgroundMusic);
	ini.SetValue(section, "AudioOutputBufferSize", IAudioOutputBufferSize[AudioOutputBufferSizeIndex].c_str());
	SAVE_ENUM_VALUE(section, "VoicePassthrough", VoicePassthrough);
}

void Ini::LoadLyricsSettings()
{
	static const char * section = "Lyrics";

	LyricsFont    = LOOKUP_ENUM_VALUE(LyricsFontType,    section, "LyricsFont", LyricsFontType::Plain);
	LyricsEffect  = LOOKUP_ENUM_VALUE(LyricsEffectType,  section, "LyricsEffect", LyricsEffectType::Shift);
	NoteLines     = LOOKUP_ENUM_VALUE(Switch,            section, "NoteLines", Switch::On);

	DefaultEncoding
	              = LOOKUP_ENUM_VALUE(Encoding,          section, "Encoding", Encoding::Auto);
}

void Ini::SaveLyricsSettings()
{
	static const char * section = "Lyrics";

	SAVE_ENUM_VALUE(section, "LyricsFont", LyricsFont);
	SAVE_ENUM_VALUE(section, "LyricsEffect", LyricsEffect);
	SAVE_ENUM_VALUE(section, "NoteLines", NoteLines);
	SAVE_ENUM_VALUE(section, "DefaultEncoding", DefaultEncoding);
}

void Ini::LoadAdvancedSettings()
{
	static const char * section = "Advanced";

	LoadAnimation = LOOKUP_ENUM_VALUE(Switch,            section, "LoadAnimation", Switch::On);
	ScreenFade    = LOOKUP_ENUM_VALUE(Switch,            section, "ScreenFade", Switch::On);
	AskBeforeDel
	              = LOOKUP_ENUM_VALUE(Switch,            section, "AskBeforeDel", Switch::On);
	OnSongClick   = LOOKUP_ENUM_VALUE(SongClickType,     section, "OnSongClick", SongClickType::Sing);
	LineBonus     = LOOKUP_ENUM_VALUE(Switch,            section, "LineBonus", Switch::On);
	PartyPopup    = LOOKUP_ENUM_VALUE(Switch,            section, "PartyPopup", Switch::On);
	SyncTo        = LOOKUP_ENUM_VALUE(SyncToType,        section, "SyncTo", SyncToType::Music);
}

void Ini::SaveAdvancedSettings()
{
	static const char * section = "Advanced";

	SAVE_ENUM_VALUE(section, "LoadAnimation", LoadAnimation);
	SAVE_ENUM_VALUE(section, "ScreenFade", ScreenFade);
	SAVE_ENUM_VALUE(section, "AskBeforeDel", AskBeforeDel);

	SAVE_ENUM_VALUE(section, "OnSongClick", OnSongClick);
	SAVE_ENUM_VALUE(section, "LineBonus", LineBonus);
	SAVE_ENUM_VALUE(section, "PartyPopup", PartyPopup);
	SAVE_ENUM_VALUE(section, "SyncTo", SyncTo);
}

void Ini::LoadControllerSettings()
{
	static const char * section = "Controller";

	Joypad        = LOOKUP_ENUM_VALUE(Switch,            section, "Joypad", Params.Joypad ? Switch::On : Switch::Off);
	Mouse         = LOOKUP_ARRAY_INDEX(IMouse,           section, "Mouse", Params.Joypad ? 0 /* off */ : 2 /* software cursor */);
}

void Ini::SaveControllerSettings()
{
	static const char * section = "Controller";

	SAVE_ENUM_VALUE(section, "Joypad", Joypad);
	ini.SetValue(section, "Mouse", IMouse[Mouse].c_str());
}

void Ini::LoadInputDeviceConfig()
{
	static const std::string deviceNameKey("DeviceName");
	const char * section = "Record";

	// Clear the device map
	InputDevices.clear();

	const CSimpleIniA::TKeyVal * sectionKeys = ini.GetSection(section);
	if (sectionKeys == NULL)
		return;

	for (CSimpleIniA::TKeyVal::const_iterator itr = sectionKeys->begin(); itr != sectionKeys->end(); ++itr)
	{
		std::string key = itr->first.pItem;
		if (key.length() < deviceNameKey.length()
			|| key.substr(0, deviceNameKey.length()) != deviceNameKey)
			continue;

		char buff[20];
		int deviceNo;
		sscanf(key.c_str(), "%10s[%d]", buff, &deviceNo);

		InputDeviceConfig device;
		device.Name = itr->second;

		snprintf(buff, 20, "Input[%d]", deviceNo);
		device.Input = ini.GetLongValue(section, buff, 0);

		snprintf(buff, 20, "Latency[%d]", deviceNo);
		device.Latency = ini.GetLongValue(section, buff, LATENCY_AUTODETECT);

		for (int channelIndex = 0; ; channelIndex++)
		{
			snprintf(buff, 20, "Channel%d[%d]", channelIndex + 1, deviceNo);
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
	const char * section = "Record";

	// Remove entire recording device section.
	// As we still have the config file loaded in memory,
	// we still have old device config which may need to be removed.
	// Removing the entire section & replacing it ensures there's no lingering data.
	ini.Delete(section, NULL, true);

	for (InputDeviceConfigMap::const_iterator itr = InputDevices.begin();
		itr != InputDevices.end();
		++itr)
	{
		char buff[20];
		const InputDeviceConfig& device = itr->second;
		int deviceNo = itr->first;

		snprintf(buff, 20, "DeviceName[%d]", deviceNo);
		ini.SetValue(section, buff, device.Name.c_str());

		snprintf(buff, 20, "Input[%d]", deviceNo);
		ini.SetLongValue(section, buff, device.Input);

		snprintf(buff, 20, "Latency[%d]", deviceNo);
		ini.SetLongValue(section, buff, device.Latency);

		for (ChannelToPlayerMap::const_iterator channelItr = device.ChannelMap.begin();
			channelItr != device.ChannelMap.end();
			++channelItr)
		{
			snprintf(buff, 20, "Channel%d[%d]", channelItr->first, deviceNo);
			ini.SetLongValue(section, buff, channelItr->second);
		}
	}
}

void Ini::LoadThemes()
{
	if (sThemes.GetThemeCount() == 0)
		return sLog.Critical("No themes are loaded.");

	static const char * section = "Themes";

	// TODO: Clean this up. It would be preferable to only store the theme & skin names here.
	std::string themeName = ini.GetValue(section, "Theme", DEFAULT_THEME);

	Theme = sThemes.LookupThemeDefault(themeName, DEFAULT_THEME);
	if (Theme == NULL)
		return sLog.Critical("Theme not found, and default theme does not exist.");

	if (Theme->DefaultSkin == NULL)
		return sLog.Critical("Specified theme doesn't have a default skin.");

	std::string skinName = ini.GetValue(section, "Skin", Theme->DefaultSkin->Name.c_str());
	Skin = sSkins.LookupSkinForTheme(skinName, Theme->Name);
	if (Skin == NULL)
		Skin = Theme->DefaultSkin; // NOTE: Checked earlier

	ThemeColor = LOOKUP_ENUM_VALUE(Color, section, "Color", Skin->DefaultColor);
}

void Ini::SaveThemes()
{
	static const char * section = "Themes";

	// TODO: Clean this up. It would be preferable to only store the theme & skin names here.
	ini.SetValue(section, "Theme", Theme->Name.c_str());
	ini.SetValue(section, "Skin", Theme->DefaultSkin->Name.c_str());
	SAVE_ENUM_VALUE(section, "Color", Theme->DefaultSkin->DefaultColor);
}

void Ini::LoadPaths()
{
	const char * section = "Directories";
	const CSimpleIniA::TKeyVal * sectionKeys = ini.GetSection(section);
	if (sectionKeys == NULL)
		return;

	for (CSimpleIniA::TKeyVal::const_iterator itr = sectionKeys->begin(); itr != sectionKeys->end(); ++itr)
		AddSongPath(path(itr->second));
}

void Ini::SavePaths()
{
	const char * section = "Directories";

	// Remove entire directories section.
	// As we still have the config file loaded in memory,
	// we still have old directories which may need to be removed.
	// Removing the entire section & replacing it ensures there's no lingering data.
	ini.Delete(section, NULL, true);

	int n = 1;
	for (PathSet::const_iterator itr = SongPaths.begin(); itr != SongPaths.end(); ++itr)
	{
		char buff[20];
		_snprintf(buff, 20, "SongDir%d", n++);
		ini.SetValue(section, buff, itr->generic_string().c_str());
	}
}

Ini::~Ini()
{
}
