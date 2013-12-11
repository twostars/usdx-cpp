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

#ifndef _INI_H
#define _INI_H
#pragma once

struct InputDeviceConfig
{
	tstring Name;    // Name of the input device.
	int     Input;   // Index of the input source to use for recording.
	int     Latency; // Latency in ms, or LATENCY_AUTODETECT for default.

    /**
     * Mapping of recording channels to players, e.g. ChannelToPlayerMap[0] = 2
     * maps the channel 0 (left) to player 2.
     * A player index of 0 (CHANNEL_OFF) means that the channel is not assigned
     * to any player (the channel is off).
     */
	std::map<int, int> ChannelToPlayerMap;
};


// Constants for InputDeviceConfig
static const int CHANNEL_OFF = 0;         // for field ChannelToPlayerMap
static const int LATENCY_AUTODETECT = -1; // for field Latency

#define LOOKUP_ENUM_VALUE(type, section, key, defaultValue) \
	type::String2Enum(ini.GetValue(section, key, type::Enum2String(defaultValue).c_str()))

#define LOOKUP_ARRAY_INDEX(arr, section, key, defaultOffset) \
	GET_INDEX(arr, ini.GetValue(section, key, arr[defaultOffset].c_str()))


typedef std::pair<int, int> ResolutionWH;
typedef std::map<ResolutionWH, tstring> ResolutionMap;
typedef std::map<tstring, ResolutionWH, tstring_ci> ReverseResolutionMap;

// User configuration
class Ini : public Singleton<Ini>
{
public:
	Ini();
	void Load();

private:
	void LoadProfileSettings(CSimpleIni& ini);
	void LoadGameSettings(CSimpleIni& ini);
	void LoadGraphicsSettings(CSimpleIni& ini);
	void LoadScreenModes(CSimpleIni& ini);
	void LoadSoundSettings(CSimpleIni& ini);
	void LoadLyricsSettings(CSimpleIni& ini);
	void LoadAdvancedSettings(CSimpleIni& ini);
	void LoadControllerSettings(CSimpleIni& ini);
	void LoadInputDeviceConfig(CSimpleIni& ini);
	void LoadThemes(CSimpleIni& ini);
	void LoadPaths(CSimpleIni& ini);

	void AddResolution(int width, int height);

public:
	void Save();
	~Ini();

public:
	tstring Name[MAX_PROFILE_NAMES];

	// Templates for names
	tstring NameTeam[MAX_TEAMS];
	tstring NameTemplate[MAX_PROFILE_NAMES];

	// Game
	int Players;
	eDifficultyType Difficulty;
	tstring LanguageName;
	eSwitch Tabs;
	eSwitch TabsAtStartup;
	eSortingType Sorting;
	eSwitch Debug;

	// Graphics
	int Screens;
	ResolutionWH Resolution;
	int Depth;
	eVisualizerOption VisualizerOption;
	eSwitch FullScreen;
	int TextureSize;
	eSingWindowType SingWindow;
	eSwitch Oscilloscope;
	eSwitch Spectrum;
	eSwitch Spectrograph;
	int MovieSize;
	eSwitch VideoPreview;
	eSwitch VideoEnabled;

	// Sound
	int MicBoost;
	eSwitch ClickAssist;
	eSwitch BeatClick;
	eSwitch SavePlayback;
	int ThresholdIndex;
	int AudioOutputBufferSizeIndex;
	eSwitch VoicePassthrough;
	eSyncToType SyncTo;

	// Song preview
	int PreviewVolume;
	int PreviewFading;

	// Lyrics
	eLyricsFontType LyricsFont;
	eLyricsEffectType LyricsEffect;
	eSwitch NoteLines;

	// Themes
	struct ThemeEntry * Theme;
	struct SkinEntry  * Skin;
	eColor ThemeColor;
	eSwitch BackgroundMusic;

	// Record
	std::map<int, InputDeviceConfig> InputDeviceConfigMap;

	// Advanced
	eSwitch LoadAnimation;
	eSwitch EffectSing;
	eSwitch ScreenFade;
	eSwitch AskBeforeDel;
	eSongClickType OnSongClick;
	eSwitch LineBonus;
	eSwitch PartyPopup;

	// Controller
	eSwitch Joypad;
	int Mouse;

	// Default encoding for texts (lyrics, song name, ...)
	eEncoding DefaultEncoding;

	ResolutionMap        LoadedResolutions;
	ReverseResolutionMap ResolutionNameMap;
};

#define sIni (Ini::getSingleton())

#endif
