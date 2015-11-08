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
#include "PathUtils.h"
#include "Themes.h"
#include "Log.h"
#include "Skins.h"
#include "ThemeDefines.h"
#include "Ini.h"
#include "Language.h"
#include "TextGL.h"

#include "../shared/color_utils.h"

using namespace boost::filesystem;

initialiseSingleton(Themes);

extern path ThemePath;

Themes::Themes()
	: ini(true)
	, _lastThemeBasic(NULL)
{
	#define INIT_THEME(name) name = NULL

	INIT_THEME(Loading);
	INIT_THEME(Main);
	INIT_THEME(Name);
	INIT_THEME(Level);
	INIT_THEME(Song);
	INIT_THEME(Sing);
	INIT_THEME(LyricBar);
	INIT_THEME(Score);
	INIT_THEME(Top5);
	INIT_THEME(Options);
	INIT_THEME(OptionsGame);
	INIT_THEME(OptionsGraphics);
	INIT_THEME(OptionsSound);
	INIT_THEME(OptionsLyrics);
	INIT_THEME(OptionsThemes);
	INIT_THEME(OptionsRecord);
	INIT_THEME(OptionsAdvanced);
	INIT_THEME(Edit);
	INIT_THEME(ErrorPopup);
	INIT_THEME(CheckPopup);
	INIT_THEME(SongMenu);
	INIT_THEME(SongJumpTo);
	INIT_THEME(PartyNewRound);
	INIT_THEME(PartyWin);
	INIT_THEME(PartyScore);
	INIT_THEME(PartyOptions);
	INIT_THEME(PartyPlayer);
	INIT_THEME(PartyRounds);
	INIT_THEME(StatMain);
	INIT_THEME(StatDetail);
	INIT_THEME(Playlist);

	#undef INIT_THEME

	LoadList();
}

/**
 * Frees existing theme objects (if any), and recreates.
 */
void Themes::CreateThemeObjects()
{
	#define LOAD_THEME(name) \
		if (name != NULL) \
			delete name; \
		name = new Theme ## name()

	LOAD_THEME(Loading);
	LOAD_THEME(Main);
	LOAD_THEME(Name);
	LOAD_THEME(Level);
	LOAD_THEME(Song);
	LOAD_THEME(Sing);
	LOAD_THEME(LyricBar);
	LOAD_THEME(Score);
	LOAD_THEME(Top5);
	LOAD_THEME(Options);
	LOAD_THEME(OptionsGame);
	LOAD_THEME(OptionsGraphics);
	LOAD_THEME(OptionsSound);
	LOAD_THEME(OptionsLyrics);
	LOAD_THEME(OptionsThemes);
	LOAD_THEME(OptionsRecord);
	LOAD_THEME(OptionsAdvanced);
	LOAD_THEME(Edit);
	LOAD_THEME(ErrorPopup);
	LOAD_THEME(CheckPopup);
	LOAD_THEME(SongMenu);
	LOAD_THEME(SongJumpTo);
	LOAD_THEME(PartyNewRound);
	LOAD_THEME(PartyWin);
	LOAD_THEME(PartyScore);
	LOAD_THEME(PartyOptions);
	LOAD_THEME(PartyPlayer);
	LOAD_THEME(PartyRounds);
	LOAD_THEME(StatMain);
	LOAD_THEME(StatDetail);
	LOAD_THEME(Playlist);

	#undef LOAD_THEME
}

void Themes::LoadList()
{
	directory_iterator end;

	try
	{
		sLog.Status(
			"Themes::LoadList",
			"Searching for themes in directory: %s",
			ThemePath.generic_string().c_str());

		// Clear the existing themes list.
		_themes.clear();

		for (directory_iterator itr(ThemePath); itr != end; ++itr)
		{
			const path& p = itr->path();
			if (is_directory(p)
				|| !p.has_extension()
				|| p.extension() != ".ini")
				continue;

			sLog.Status(
				"Themes::LoadList",
				"Found theme %s",
				p.stem().generic_string().c_str());

			LoadHeader(p);
		}
	}
	catch (filesystem_error)
	{
		sLog.Critical(
			"Themes::LoadList",
			"Could not access themes directory (%s)", 
			ThemePath.generic_string().c_str());
	}
}

void Themes::LoadHeader(const path& iniFile)
{
	CSimpleIniA ini(true);
	SI_Error result = ini.LoadFile(iniFile.native().c_str());
	if (result != SI_OK)
	{
		return sLog.Warn(
			"Themes::LoadHeader",
			"Failed to load INI (%s)",
			iniFile.generic_string().c_str());
	}

	const char * section = "Theme";

	ThemeEntry theme;
	std::string themeVersion;
	const char * creator;

	theme.Path = iniFile.branch_path();
	theme.FileName = iniFile;
	theme.Name = ini.GetValue(section, "Name", "(no name)");
	themeVersion = ini.GetValue(section, "US_Version", "no version");

	// Attempt to lookup theme's creator.
	// NOTE: Current USDX loads "Creator", but older themes use "Author".
	// Attempt to fallback to "Author" if "Creator" isn't found.
	creator				= ini.GetValue(section, "Creator");
	if (creator == NULL)
		creator			= ini.GetValue(section, "Author", "");

	theme.Creator		= creator;

	if (themeVersion != USDX_THEME_VERSION)
	{
		return sLog.Warn(
			"Theme::LoadHeader",
			"Wrong version (%s) for theme %s.",
			themeVersion.c_str(), theme.Name.c_str());
	}

	theme.DefaultSkin = sSkins.LookupSkinForTheme(theme.Name);
	if (theme.DefaultSkin == NULL)
	{
		return sLog.Warn(
			"Theme::LoadHeader",
			"Skin does not exist for theme %s.",
			theme.Name.c_str());
	}

	_themes.insert(std::make_pair(theme.Name, theme));
}

void Themes::LoadTheme(ThemeEntry * theme, eColor color)
{
	if (theme == NULL)
		sLog.Critical("Theme not set.");

	sLog.Status(
		"Themes::LoadTheme",
		"Loading: %s",
		theme->FileName.generic_string().c_str());

	SI_Error result = ini.LoadFile(theme->FileName.c_str());
	if (result != SI_OK)
		sLog.Critical("Unable to load theme config file.");

	CreateThemeObjects();

	sSkins.LoadSkin(sIni.Skin, color);

	LoadColors();

	// Loading~
	LoadThemeBasic(Loading, "Loading");
	LoadThemeText(Loading->TextLoading, "LoadingTextLoading");
	LoadThemeStatic(Loading->StaticAnimation, "LoadingStaticAnimation");

	// Main~
	LoadThemeBasic(Main, "Main");
	LoadThemeText(Main->TextDescription, "MainTextDescription");
	LoadThemeText(Main->TextDescriptionLong, "MainTextDescriptionLong");
	LoadThemeButton(Main->ButtonSolo, "MainButtonSolo");
	LoadThemeButton(Main->ButtonMulti, "MainButtonMulti");
	LoadThemeButton(Main->ButtonStats, "MainButtonStats");
	LoadThemeButton(Main->ButtonEditor, "MainButtonEditor");
	LoadThemeButton(Main->ButtonOptions, "MainButtonOptions");
	LoadThemeButton(Main->ButtonExit, "MainButtonExit");

	// Main description translation
	Main->Description[0]     = __T("SING_SING");
	Main->DescriptionLong[0] = __T("SING_SING_DESC");
	Main->Description[1]     = __T("SING_MULTI");
	Main->DescriptionLong[1] = __T("SING_MULTI_DESC");
	Main->Description[2]     = __T("SING_STATS");
	Main->DescriptionLong[2] = __T("SING_STATS_DESC");
	Main->Description[3]     = __T("SING_EDITOR");
	Main->DescriptionLong[3] = __T("SING_EDITOR_DESC");
	Main->Description[4]     = __T("SING_GAME_OPTIONS");
	Main->DescriptionLong[4] = __T("SING_GAME_OPTIONS_DESC");
	Main->Description[5]     = __T("SING_EXIT");
	Main->DescriptionLong[5] = __T("SING_EXIT_DESC");

	// Active description
	Main->TextDescription.Text     = Main->Description[0];
	Main->TextDescriptionLong.Text = Main->DescriptionLong[0];

	// Name~
	LoadThemeBasic(Name, "Name");

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		char buff[20];
		_snprintf(buff, 20, "NameButtonPlayer%d", i + 1);
		LoadThemeButton(Name->ButtonPlayer[i], buff);
	}
	
	// Level~
	LoadThemeBasic(Level, "Level");
	LoadThemeButton(Level->ButtonEasy,   "LevelButtonEasy");
	LoadThemeButton(Level->ButtonMedium, "LevelButtonMedium");
	LoadThemeButton(Level->ButtonHard,   "LevelButtonHard");

	// Song~
	LoadThemeBasic(Song, "Song");
	LoadThemeText(Song->TextArtist,   "SongTextArtist");
	LoadThemeText(Song->TextTitle,    "SongTextTitle");
	LoadThemeText(Song->TextNumber,   "SongTextNumber");

	// Video icon
	LoadThemeStatic(Song->VideoIcon,  "SongVideoIcon");

	// Medley icons
	LoadThemeStatic(Song->MedleyIcon, "SongMedleyIcon");
	LoadThemeStatic(Song->CalculatedMedleyIcon, "SongCalculatedMedleyIcon");

	// Show cat in top-left
	LoadThemeStatic(Song->StaticCat,  "SongStaticCat");
	LoadThemeText(Song->TextCat,      "SongTextCat");

	// Load cover pos & size from theme
	Song->Cover.X           = ini.GetLongValue("SongCover", "X", 300);
	Song->Cover.Y           = ini.GetLongValue("SongCover", "Y", 190);
	Song->Cover.W           = ini.GetLongValue("SongCover", "W", 300);
	Song->Cover.H           = ini.GetLongValue("SongCover", "H", 200);
	Song->Cover.Style       = ini.GetLongValue("SongCover", "Style", 4);
	Song->Cover.Reflections = ini.GetBoolValue("SongCover", "Reflections", false);

	// Equalizer
	LoadThemeEqualizer(Song->Equalizer, "SongEqualizer");

	// Party & non-party specific statics & texts
	LoadThemeStatics(Song->StaticParty, "SongStaticParty");
	LoadThemeTexts(Song->TextParty, "SongTextParty");
	LoadThemeStatics(Song->StaticNonParty, "SongStaticNonParty");
	LoadThemeTexts(Song->TextNonParty, "SongTextNonParty");

	// Party mode
	for (int teamNo = 0; teamNo < MAX_TEAMS; teamNo++)
	{
		char buff[40];

		for (int jokerNo = 0; jokerNo < MAX_JOKERS; jokerNo++)
		{
			snprintf(buff, 40, "StaticTeam%dJoker%d", teamNo + 1, jokerNo + 1);
			LoadThemeStatic(Song->StaticTeamJoker[teamNo][jokerNo], buff);
		}
	}

	// Lyric bar
	LyricBar->UpperX = ini.GetLongValue("SingLyricsUpperBar", "X", 0);
	LyricBar->UpperW = ini.GetLongValue("SingLyricsUpperBar", "W", 0);
	LyricBar->UpperY = ini.GetLongValue("SingLyricsUpperBar", "Y", 0);
	LyricBar->UpperH = ini.GetLongValue("SingLyricsUpperBar", "H", 0);
	LyricBar->IndicatorYOffset = ini.GetLongValue("SingLyricsUpperBar", "IndicatorYOffset", 0);
	LyricBar->LowerX = ini.GetLongValue("SingLyricsLowerBar", "X", 0);
	LyricBar->LowerW = ini.GetLongValue("SingLyricsLowerBar", "W", 0);
	LyricBar->LowerY = ini.GetLongValue("SingLyricsLowerBar", "Y", 0);
	LyricBar->LowerH = ini.GetLongValue("SingLyricsLowerBar", "H", 0);

	// Sing~
	LoadThemeBasic(Sing, "Sing");

	// Time bar
	LoadThemeStatic(Sing->StaticTimeProgress, "SingTimeProgress");
	LoadThemeText(Sing->TextTimeText, "SingTimeText");

	// Moveable singbar
	LoadThemeStatic(Sing->StaticP1SingBar, "SingP1SingBar");
	LoadThemeStatic(Sing->StaticP1TwoPSingBar, "SingP1TwoPSingBar");
	LoadThemeStatic(Sing->StaticP1ThreePSingBar, "SingP1ThreePSingBar");
	LoadThemeStatic(Sing->StaticP2RSingBar, "SingP2RSingBar");
	LoadThemeStatic(Sing->StaticP2MSingBar, "SingP2MSingBar");
	LoadThemeStatic(Sing->StaticP3SingBar, "SingP3SingBar");

	LoadThemeStatic(Sing->StaticP1, "SingP1Static");
	LoadThemeText(Sing->TextP1, "SingP1Text");
	LoadThemeStatic(Sing->StaticP1ScoreBG, "SingP1Static2");
	LoadThemeText(Sing->TextP1Score, "SingP1TextScore");

	// This one is shown in 2/4 player mode if it exists.
	// If not, the player equivalents are used.
	if (ini.GetSectionSize("SingP1TwoPTextScore") > 0)
	{
		LoadThemeStatic(Sing->StaticP1TwoP, "SingP1TwoPStatic");
		LoadThemeText(Sing->TextP1TwoP, "SingP1TwoPText");
		LoadThemeStatic(Sing->StaticP1TwoPScoreBG, "SingP1TwoPStatic2");
		LoadThemeText(Sing->TextP1TwoPScore, "SingP1TwoPTextScore");
	}
	else
	{
		Sing->StaticP1TwoP = Sing->StaticP1;
		Sing->TextP1TwoP = Sing->TextP1;
		Sing->StaticP1TwoPScoreBG = Sing->StaticP1ScoreBG;
		Sing->TextP1TwoPScore = Sing->TextP1Score;
	}

	// This one is shown in 3/6 player mode if it exists.
	// If not, the player equivalents are used.
	if (ini.GetSectionSize("SingP1ThreePTextScore") > 0)
	{
		LoadThemeStatic(Sing->StaticP1ThreeP, "SingP1ThreePStatic");
		LoadThemeText(Sing->TextP1ThreeP, "SingP1ThreeText");
		LoadThemeStatic(Sing->StaticP1ThreePScoreBG, "SingP1ThreePStatic2");
		LoadThemeText(Sing->TextP1ThreePScore, "SingP1ThreePTextScore");
	}
	else
	{
		Sing->StaticP1ThreeP = Sing->StaticP1;
		Sing->TextP1ThreeP = Sing->TextP1;
		Sing->StaticP1ThreePScoreBG = Sing->StaticP1ScoreBG;
		Sing->TextP1ThreePScore = Sing->TextP1Score;
	}

	LoadThemeStatic(Sing->StaticP2R, "SingP2RStatic");
	LoadThemeText(Sing->TextP2R, "SingP2RText");
	LoadThemeStatic(Sing->StaticP2RScoreBG, "SingP2RStatic2");
	LoadThemeText(Sing->TextP2RScore, "SingP2RTextScore");

	LoadThemeStatic(Sing->StaticP2M, "SingP2MStatic");
	LoadThemeText(Sing->TextP2M, "SingP2MText");
	LoadThemeStatic(Sing->StaticP2MScoreBG, "SingP2MStatic2");
	LoadThemeText(Sing->TextP2MScore, "SingP2MTextScore");

	LoadThemeStatic(Sing->StaticP3R, "SingP3RStatic");
	LoadThemeText(Sing->TextP3R, "SingP3RText");
	LoadThemeStatic(Sing->StaticP3RScoreBG, "SingP3RStatic2");
	LoadThemeText(Sing->TextP3RScore, "SingP3RTextScore");

	LoadThemeStatic(Sing->StaticSongName, "SingSongNameStatic");
	LoadThemeText(Sing->TextSongName, "SingSongNameText");

	// Line bonus texts
	Sing->LineBonusText[0] = __T("POPUP_AWFUL");
	Sing->LineBonusText[1] = Sing->LineBonusText[0];
	Sing->LineBonusText[2] = __T("POPUP_POOR");
	Sing->LineBonusText[3] = __T("POPUP_BAD");
	Sing->LineBonusText[4] = __T("POPUP_NOTBAD");
	Sing->LineBonusText[5] = __T("POPUP_GOOD");
	Sing->LineBonusText[6] = __T("POPUP_GREAT");
	Sing->LineBonusText[7] = __T("POPUP_AWESOME");
	Sing->LineBonusText[8] = __T("POPUP_PERFECT");

	// Pause popup
	LoadThemeStatic(Sing->PausePopup, "PausePopUpStatic");

	// Score~
	LoadThemeBasic(Score, "Score");
	LoadThemeText(Score->TextArtist,      "ScoreTextArtist");
	LoadThemeText(Score->TextTitle,       "ScoreTextTitle");
	LoadThemeText(Score->TextArtistTitle, "ScoreTextArtistTitle");

	#define F(format, ...) \
		snprintf(buff, SDL_arraysize(buff), format, __VA_ARGS__)

	for (int playerNo = 1; playerNo <= MAX_PLAYERS; playerNo++)
	{
		char buff[40];
		int playerID = playerNo - 1;

		#define P(format) \
			F(format, playerNo)

		P("ScorePlayer%dStatic");         LoadThemeStatics(Score->PlayerStatic[playerID], buff);
		P("ScorePlayer%dText");           LoadThemeTexts(Score->PlayerTexts[playerID], buff);

		P("ScoreTextName%d");             LoadThemeText(Score->TextName[playerID], buff);
		P("ScoreTextScore%d");            LoadThemeText(Score->TextScore[playerID], buff);
		P("ScoreTextNotes%d");            LoadThemeText(Score->TextNotes[playerID], buff);
		P("ScoreTextNotesScore%d");       LoadThemeText(Score->TextNotesScore[playerID], buff);
		P("ScoreTextLineBonus%d");        LoadThemeText(Score->TextLineBonus[playerID], buff);
		P("ScoreTextLineBonusScore%d");   LoadThemeText(Score->TextLineBonusScore[playerID], buff);
		P("ScoreTextGoldenNotes%d");      LoadThemeText(Score->TextGoldenNotes[playerID], buff);
		P("ScoreTextGoldenNotesScore%d"); LoadThemeText(Score->TextGoldenNotesScore[playerID], buff);
		P("ScoreTextTotal%d");            LoadThemeText(Score->TextTotal[playerID], buff);
		P("ScoreTextTotalScore%d");       LoadThemeText(Score->TextTotalScore[playerID], buff);

		P("ScoreStaticBoxLightest%d");    LoadThemeStatic(Score->StaticBoxLightest[playerID], buff);
		P("ScoreStaticBoxLight%d");       LoadThemeStatic(Score->StaticBoxLight[playerID], buff);
		P("ScoreStaticBoxDark%d");        LoadThemeStatic(Score->StaticBoxDark[playerID], buff);

		P("ScoreStaticBackLevel%d");      LoadThemeStatic(Score->StaticBackLevel[playerID], buff);
		P("ScoreStaticBackLevelRound%d"); LoadThemeStatic(Score->StaticBackLevelRound[playerID], buff);
		P("ScoreStaticLevel%d");          LoadThemeStatic(Score->StaticLevel[playerID], buff);
		P("ScoreStaticLevelRound%d");     LoadThemeStatic(Score->StaticLevelRound[playerID], buff);
		P("ScoreStaticPlayerIdBox%d");    LoadThemeStatic(Score->StaticPlayerIdBox[playerID], buff);

		P("ScoreStaticRatingPicture%d");  LoadThemeStatic(Score->StaticRatings[playerID], buff);

		#undef P
	}

	LoadThemeStatic(Score->StaticNavigate, "ScoreStaticNavigate");
	LoadThemeText(Score->TextNavigate, "ScoreTextNavigate");
	
	// Top5~
	LoadThemeBasic(Top5, "Top5");
	LoadThemeText(Top5->TextLevel, "Top5TextLevel");
	LoadThemeText(Top5->TextArtistTitle, "Top5TextArtistTitle");
	LoadThemeStatics(Top5->StaticNumber, "Top5StaticNumber");
	LoadThemeTexts(Top5->TextNumber, "Top5TextNumber");
	LoadThemeTexts(Top5->TextName, "Top5TextName");
	LoadThemeTexts(Top5->TextScore, "Top5TextScore");
	LoadThemeTexts(Top5->TextDate, "Top5TextDate");

	// Options~
	LoadThemeBasic(Options, "Options");
	LoadThemeButton(Options->ButtonGame, "OptionsButtonGame");
	LoadThemeButton(Options->ButtonGraphics, "OptionsButtonGraphics");
	LoadThemeButton(Options->ButtonSound, "OptionsButtonSound");
	LoadThemeButton(Options->ButtonLyrics, "OptionsButtonLyrics");
	LoadThemeButton(Options->ButtonThemes, "OptionsButtonThemes");
	LoadThemeButton(Options->ButtonRecord, "OptionsButtonRecord");
	LoadThemeButton(Options->ButtonAdvanced, "OptionsButtonAdvanced");
	LoadThemeButton(Options->ButtonExit, "OptionsButtonExit");
	LoadThemeText(Options->TextDescription, "OptionsTextDescription");

	Options->Description[0] = __T("SING_OPTIONS_GAME_DESC");
	Options->Description[1] = __T("SING_OPTIONS_GRAPHICS_DESC");
	Options->Description[2] = __T("SING_OPTIONS_SOUND_DESC");
	Options->Description[3] = __T("SING_OPTIONS_LYRICS_DESC");
	Options->Description[4] = __T("SING_OPTIONS_THEMES_DESC");
	Options->Description[5] = __T("SING_OPTIONS_RECORD_DESC");
	Options->Description[6] = __T("SING_OPTIONS_ADVANCED_DESC");
	Options->Description[7] = __T("SING_OPTIONS_EXIT");

	// Set active description
	Options->TextDescription.Text = Options->Description[0];

	// Options: game~
	LoadThemeBasic(OptionsGame, "OptionsGame");
	LoadThemeSelectSlide(OptionsGame->SelectPlayers, "OptionsGameSelectPlayers");
	LoadThemeSelectSlide(OptionsGame->SelectDifficulty, "OptionsGameSelectDifficulty");
	LoadThemeSelectSlide(OptionsGame->SelectLanguage, "OptionsGameSelectSlideLanguage");
	LoadThemeSelectSlide(OptionsGame->SelectTabs, "OptionsGameSelectTabs");
	LoadThemeSelectSlide(OptionsGame->SelectSorting, "OptionsGameSelectSlideSorting");
	LoadThemeSelectSlide(OptionsGame->SelectDebug, "OptionsGameSelectDebug");
	LoadThemeButton(OptionsGame->ButtonExit, "OptionsGameButtonExit");

	// Options: graphics~
	LoadThemeBasic(OptionsGraphics, "OptionsGraphics");
	LoadThemeSelectSlide(OptionsGraphics->SelectFullscreen, "OptionsGraphicsSelectFullscreen");
	LoadThemeSelectSlide(OptionsGraphics->SelectResolution, "OptionsGraphicsSelectSlideResolution");
	LoadThemeSelectSlide(OptionsGraphics->SelectDepth, "OptionsGraphicsSelectDepth");
	LoadThemeSelectSlide(OptionsGraphics->SelectVisualizer, "OptionsGraphicsSelectVisualizer");
	LoadThemeSelectSlide(OptionsGraphics->SelectOscilloscope, "OptionsGraphicsSelectOscilloscope");
	LoadThemeSelectSlide(OptionsGraphics->SelectLineBonus, "OptionsGraphicsSelectLineBonus");
	LoadThemeSelectSlide(OptionsGraphics->SelectMovieSize, "OptionsGraphicsSelectMovieSize");
	LoadThemeButton(OptionsGraphics->ButtonExit, "OptionsGraphicsButtonExit");

	// Options: sound~
	LoadThemeBasic(OptionsSound, "OptionsSound");
	LoadThemeSelectSlide(OptionsSound->SelectBackgroundMusic, "OptionsSoundSelectBackgroundMusic");
	LoadThemeSelectSlide(OptionsSound->SelectMicBoost, "OptionsSoundSelectMicBoost");
	LoadThemeSelectSlide(OptionsSound->SelectClickAssist, "OptionsSoundSelectClickAssist");
	LoadThemeSelectSlide(OptionsSound->SelectBeatClick, "OptionsSoundSelectBeatClick");
	LoadThemeSelectSlide(OptionsSound->SelectThreshold, "OptionsSoundSelectThreshold");
	LoadThemeSelectSlide(OptionsSound->SelectSlidePreviewVolume, "OptionsSoundSelectSlidePreviewVolume");
	LoadThemeSelectSlide(OptionsSound->SelectSlidePreviewFading, "OptionsSoundSelectSlidePreviewFading");
	LoadThemeSelectSlide(OptionsSound->SelectSlideVoicePassthrough, "OptionsSoundSelectVoicePassthrough");
	LoadThemeButton(OptionsSound->ButtonExit, "OptionsSoundButtonExit");

	// Options: lyrics~
	LoadThemeBasic(OptionsLyrics, "OptionsLyrics");
	LoadThemeSelectSlide(OptionsLyrics->SelectLyricsFont, "OptionsLyricsSelectLyricsFont");
	LoadThemeSelectSlide(OptionsLyrics->SelectLyricsEffect, "OptionsLyricsSelectLyricsEffect");
	LoadThemeSelectSlide(OptionsLyrics->SelectNoteLines, "OptionsLyricsSelectNoteLines");
	LoadThemeButton(OptionsLyrics->ButtonExit, "OptionsLyricsButtonExit");
	
	// Options: themes~
	LoadThemeBasic(OptionsThemes, "OptionsThemes");
	LoadThemeSelectSlide(OptionsThemes->SelectTheme, "OptionsThemesSelectTheme");
	LoadThemeSelectSlide(OptionsThemes->SelectSkin, "OptionsThemesSelectSkin");
	LoadThemeSelectSlide(OptionsThemes->SelectColor, "OptionsThemesSelectColor");
	LoadThemeButton(OptionsThemes->ButtonExit, "OptionsThemesButtonExit");

	// Options: record~
	LoadThemeBasic(OptionsRecord, "OptionsRecord");
	LoadThemeSelectSlide(OptionsRecord->SelectSlideCard, "OptionsRecordSelectSlideCard");
	LoadThemeSelectSlide(OptionsRecord->SelectSlideInput, "OptionsRecordSelectSlideInput");
	LoadThemeSelectSlide(OptionsRecord->SelectSlideChannel, "OptionsRecordSelectSlideChannel");
	LoadThemeButton(OptionsRecord->ButtonExit, "OptionsRecordButtonExit");

	// Options: advanced~
	LoadThemeBasic(OptionsAdvanced, "OptionsAdvanced");
	LoadThemeSelectSlide(OptionsAdvanced->SelectLoadAnimation, "OptionsAdvancedSelectLoadAnimation");
	LoadThemeSelectSlide(OptionsAdvanced->SelectScreenFade, "OptionsAdvancedSelectScreenFade");
	LoadThemeSelectSlide(OptionsAdvanced->SelectEffectSing, "OptionsAdvancedSelectEffectSing");
	LoadThemeSelectSlide(OptionsAdvanced->SelectLineBonus, "OptionsAdvancedSelectLineBonus");
	LoadThemeSelectSlide(OptionsAdvanced->SelectOnSongClick, "OptionsAdvancedSelectSlideOnSongClick");
	LoadThemeSelectSlide(OptionsAdvanced->SelectAskBeforeDel, "OptionsAdvancedSelectAskBeforeDel");
	LoadThemeSelectSlide(OptionsAdvanced->SelectPartyPopup, "OptionsAdvancedSelectPartyPopup");
	LoadThemeButton(OptionsAdvanced->ButtonExit, "OptionsAdvancedButtonExit");

	// Edit menu~
	LoadThemeBasic(Edit, "Edit");
	LoadThemeButton(Edit->ButtonConvert, "EditButtonConvert");
	LoadThemeButton(Edit->ButtonExit, "EditButtonExit");
	LoadThemeText(Edit->TextDescription, "EditTextDescription");

	Edit->Description[0] = __T("SING_EDIT_BUTTON_DESCRIPTION_CONVERT");
	Edit->Description[1] = __T("SING_EDIT_BUTTON_DESCRIPTION_EXIT");

	// Set active description
	Edit->TextDescription.Text = Edit->Description[0];

	// Error popup~
	LoadThemeBasic(ErrorPopup, "ErrorPopup");
	LoadThemeButton(ErrorPopup->Button1, "ErrorPopupButton1");
	LoadThemeText(ErrorPopup->TextError, "ErrorPopupText");

	// Check popup~
	LoadThemeBasic(CheckPopup, "CheckPopup");
	LoadThemeButton(CheckPopup->Button1, "CheckPopupButton1");
	LoadThemeButton(CheckPopup->Button2, "CheckPopupButton2");
	LoadThemeText(CheckPopup->TextCheck, "CheckPopupText");
	
	// Song menu~
	LoadThemeBasic(SongMenu, "SongMenu");
	LoadThemeButton(SongMenu->Button1, "SongMenuButton1");
	LoadThemeButton(SongMenu->Button2, "SongMenuButton2");
	LoadThemeButton(SongMenu->Button3, "SongMenuButton3");
	LoadThemeButton(SongMenu->Button4, "SongMenuButton4");
	LoadThemeSelectSlide(SongMenu->SelectSlide3, "SongMenuSelectSlide3");
	LoadThemeText(SongMenu->TextMenu, "SongMenuTextMenu");

	// Song JumpTo~
	LoadThemeBasic(SongJumpTo, "SongJumpTo");
	LoadThemeButton(SongJumpTo->ButtonSearchText, "SongJumpToButtonSearchText");
	LoadThemeSelectSlide(SongJumpTo->SelectSlideType, "SongJumpToSelectSlideType");
	LoadThemeText(SongJumpTo->TextFound, "SongJumpToTextFound");

	// Translations
	SongJumpTo->IType[0] = __T("SONG_JUMPTO_TYPE1");
	SongJumpTo->IType[1] = __T("SONG_JUMPTO_TYPE2");
	SongJumpTo->IType[2] = __T("SONG_JUMPTO_TYPE3");
	SongJumpTo->SongsFound = __T("SONG_JUMPTO_SONGSFOUND");
	SongJumpTo->NoSongsFound = __T("SONG_JUMPTO_NOSONGSFOUND");
	SongJumpTo->CatText = __T("SONG_JUMPTO_CATTEXT");

	// Party: new round~
	LoadThemeBasic(PartyNewRound, "PartyNewRound");

	LoadThemeText(PartyNewRound->TextNextRound, "PartyNewRoundTextNextRound");
	LoadThemeText(PartyNewRound->TextNextRoundNo, "PartyNewRoundTextNextRoundNo");

	for (int roundNo = 0; roundNo < MAX_ROUNDS; roundNo++)
	{
		char buff[40];

		F("PartyNewRoundTextRound%d", roundNo + 1);
		LoadThemeText(PartyNewRound->TextRound[roundNo], buff);

		F("PartyNewRoundTextWinner%d", roundNo + 1);
		LoadThemeText(PartyNewRound->TextWinner[roundNo], buff);

		F("PartyNewRoundStaticRound%d", roundNo + 1);
		LoadThemeStatic(PartyNewRound->StaticRound[roundNo], buff);
	}

	for (int teamNo = 0; teamNo < MAX_TEAMS; teamNo++)
	{
		ThemePartyNewRound::Team& team = PartyNewRound->Teams[teamNo];
		char buff[40];

		F("PartyNewRoundTextNextPlayer%d", teamNo + 1);
		LoadThemeText(team.TextNextPlayer, buff);

		F("PartyNewRoundStaticNextPlayer%d", teamNo + 1);
		LoadThemeStatic(team.StaticNextPlayer, buff);

		F("PartyNewRoundTextScoreTeam%d", teamNo + 1);
		LoadThemeText(team.TextScore, buff);

		F("PartyNewRoundTextNameTeam%d", teamNo + 1);
		LoadThemeText(team.TextName, buff);

		F("PartyNewRoundTextTeam%dPlayers", teamNo + 1);
		LoadThemeText(team.TextPlayers, buff);

		F("PartyNewRoundStaticTeam%d", teamNo + 1);
		LoadThemeStatic(team.StaticTeam, buff);
	}

	// Party: score~
	LoadThemeBasic(PartyScore, "PartyScore");
	LoadThemeText(PartyScore->TextWinner, "PartyScoreTextWinner");

	for (int teamNo = 0; teamNo < MAX_TEAMS; teamNo++)
	{
		ThemePartyScore::Team& team = PartyScore->Teams[teamNo];
		char buff[40];

		F("PartyScoreTextScoreTeam%d", teamNo + 1);
		LoadThemeText(team.TextScore, buff);

		F("PartyScoreTextNameTeam%d", teamNo + 1);
		LoadThemeText(team.TextName, buff);

		F("PartyScoreStaticTeam%d", teamNo + 1);
		LoadThemeStatic(team.StaticTeam, buff);

		F("PartyScoreStaticTeam%dBG", teamNo + 1);
		LoadThemeStatic(team.StaticTeamBG, buff);

		F("PartyScoreStaticTeam%dDeco", teamNo + 1);
		LoadThemeStatic(team.StaticTeamDeco, buff);
	}

	PartyScore->DecoTextures.ChangeTextures = ini.GetBoolValue("PartyScoreDecoTextures", "ChangeTextures", false);

	PartyScore->DecoTextures.Texture[0] = ini.GetValue("PartyScoreDecoTextures", "FirstTexture", "");
	PartyScore->DecoTextures.Type   [0] = LOOKUP_ENUM_VALUE(TextureType, "PartyScoreDecoTextures", "FirstTyp", TextureType::Colorized);
	PartyScore->DecoTextures.Color  [0] = ini.GetValue("PartyScoreDecoTextures", "FirstColor", "Black");

	PartyScore->DecoTextures.Texture[1] = ini.GetValue("PartyScoreDecoTextures", "SecondTexture", "");
	PartyScore->DecoTextures.Type   [1] = LOOKUP_ENUM_VALUE(TextureType, "PartyScoreDecoTextures", "SecondTyp", TextureType::Colorized);
	PartyScore->DecoTextures.Color  [1] = ini.GetValue("PartyScoreDecoTextures", "SecondColor", "Black");

	PartyScore->DecoTextures.Texture[2] = ini.GetValue("PartyScoreDecoTextures", "ThirdTexture", "");
	PartyScore->DecoTextures.Type   [2] = LOOKUP_ENUM_VALUE(TextureType, "PartyScoreDecoTextures", "ThirdTyp", TextureType::Colorized);
	PartyScore->DecoTextures.Color  [2] = ini.GetValue("PartyScoreDecoTextures", "ThirdColor", "Black");

	// Party: options~
	LoadThemeBasic(PartyOptions, "PartyOptions");
	LoadThemeSelectSlide(PartyOptions->SelectLevel,     "PartyOptionsSelectLevel");
	LoadThemeSelectSlide(PartyOptions->SelectPlaylist,  "PartyOptionsSelectPlaylist");
	LoadThemeSelectSlide(PartyOptions->SelectPlaylist2, "PartyOptionsSelectPlaylist2");

	// Party: player~
	LoadThemeBasic(PartyPlayer, "PartyPlayer");
	LoadThemeSelectSlide(PartyPlayer->SelectTeams, "PartyPlayerSelectTeams");

	for (int teamNo = 0, playerNo = 0; teamNo < MAX_TEAMS; teamNo++)
	{
		ThemePartyPlayer::Team& team = PartyPlayer->Teams[teamNo];
		char buff[40];

		F("PartyPlayerTeam%dName", teamNo + 1);
		LoadThemeButton(team.TeamName, buff);

		F("PartyPlayerSelectPlayers%d", teamNo + 1);
		LoadThemeSelectSlide(team.SelectPlayers, buff);

		for (int i = 0; i < MAX_PLAYERS_PER_TEAM; i++, playerNo++)
		{
			F("PartyPlayerPlayer%dName", playerNo + 1);
			LoadThemeButton(team.PlayerName[i], buff);
		}
	}

	// Party: rounds~
	LoadThemeBasic(PartyRounds, "PartyRounds");

	LoadThemeSelectSlide(PartyRounds->SelectRoundCount, "PartyRoundsSelectRoundCount");
	for (int roundNo = 0; roundNo < MAX_ROUNDS; roundNo++)
	{
		char buff[40];
		F("PartyRoundsSelectRound%d", roundNo + 1);
		LoadThemeSelectSlide(PartyRounds->SelectRound[roundNo], buff);
	}

	// Stat: main~
	LoadThemeBasic(StatMain, "StatMain");
	LoadThemeButton(StatMain->ButtonScores, "StatMainButtonScores");
	LoadThemeButton(StatMain->ButtonSingers, "StatMainButtonSingers");
	LoadThemeButton(StatMain->ButtonSongs, "StatMainButtonSongs");
	LoadThemeButton(StatMain->ButtonBands, "StatMainButtonBands");
	LoadThemeButton(StatMain->ButtonExit, "StatMainButtonExit");
	LoadThemeText(StatMain->TextOverview, "StatMainTextOverview");

	// Stat: detail~
	LoadThemeBasic(StatDetail, "StatDetail");
	LoadThemeButton(StatDetail->ButtonNext, "StatDetailButtonNext");
	LoadThemeButton(StatDetail->ButtonPrev, "StatDetailButtonPrev");
	LoadThemeButton(StatDetail->ButtonReverse, "StatDetailButtonReverse");
	LoadThemeButton(StatDetail->ButtonExit, "StatDetailButtonExit");
	LoadThemeText(StatDetail->TextDescription, "StatDetailTextDescription");
	LoadThemeText(StatDetail->TextPage, "StatDetailTextPage");
	LoadThemeTexts(StatDetail->TextList, "StatDetailTextList");

	// Translate texts
	StatDetail->Description[0] = __T("STAT_DESC_SCORES");
	StatDetail->Description[1] = __T("STAT_DESC_SINGERS");
	StatDetail->Description[2] = __T("STAT_DESC_SONGS");
	StatDetail->Description[3] = __T("STAT_DESC_BANDS");

	StatDetail->DescriptionR[0] = __T("STAT_DESC_SCORES_REVERSED");
	StatDetail->DescriptionR[1] = __T("STAT_DESC_SINGERS_REVERSED");
	StatDetail->DescriptionR[2] = __T("STAT_DESC_SONGS_REVERSED");
	StatDetail->DescriptionR[3] = __T("STAT_DESC_BANDS_REVERSED");

	StatDetail->FormatStr[0] = __T("STAT_FORMAT_SCORES");
	StatDetail->FormatStr[1] = __T("STAT_FORMAT_SINGERS");
	StatDetail->FormatStr[2] = __T("STAT_FORMAT_SONGS");
	StatDetail->FormatStr[3] = __T("STAT_FORMAT_BANDS");

	StatDetail->PageStr = __T("STAT_PAGE");

	// Playlist translations~
	Playlist->CatText = __T("PLAYLIST_CATTEXT");

#undef F
}

void Themes::LoadColors()
{
	const char * section = "Colors";
	const CSimpleIniA::TKeyVal * sectionKeys = ini.GetSection(section);

	// Clear color map
	_colors.clear();

	if (sectionKeys != NULL)
	{
		for (CSimpleIniA::TKeyVal::const_iterator itr = sectionKeys->begin(); itr != sectionKeys->end(); ++itr)
		{
			RGB rgb;
			rgb.Load(itr->second);

			_colors.insert(std::make_pair(itr->first.pItem, rgb));
		}
	}

	RGB skinRGB[3];

	// Dark (0), Light (1), Lightest (2)
	skinRGB[0] = GetSystemColor(sSkins.SkinColor);
	skinRGB[1].Sqrt(skinRGB[0]);
	skinRGB[2].Sqrt(skinRGB[1]);

	_colors.insert(std::make_pair("ColorDark",     skinRGB[0]));
	_colors.insert(std::make_pair("ColorLight",    skinRGB[1]));
	_colors.insert(std::make_pair("ColorLightest", skinRGB[2]));

	eColor playerColours[MAX_PLAYERS] = 
	{
		Color::Blue, Color::Red, Color::Green,
		Color::Brown, Color::Yellow, Color::Violet
	};

	for (int i = 1; i <= MAX_PLAYERS; i++)
	{
		RGB playerRGB[3];
		char name[20];

		// Dark (0), Light (1), Lightest (2)
		playerRGB[0] = GetSystemColor(playerColours[i - 1]);
		playerRGB[1].Sqrt(playerRGB[0]);
		playerRGB[2].Sqrt(playerRGB[1]);

		snprintf(name, 20, "P%dDark", i);
		_colors.insert(std::make_pair(name, playerRGB[0]));

		snprintf(name, 20, "P%dLight", i);
		_colors.insert(std::make_pair(name, playerRGB[1]));

		snprintf(name, 20, "P%dLightest", i);
		_colors.insert(std::make_pair(name, playerRGB[2]));
	}
}

void Themes::LoadColor(RGB& rgb, const std::string& color)
{
	ColorMap::iterator itr = _colors.find(color);
	if (itr != _colors.end())
		rgb = itr->second;
	else
		rgb.R = rgb.G = rgb.B = 0.0f;
}

void Themes::LoadColor(RGB& rgb, const char * fmt, ...)
{
	char buffer[1024];
	std::string color;
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, 1024, fmt, args);
	va_end(args);

	color = buffer;
	LoadColor(rgb, color);
}

void Themes::LoadThemeBasic(ThemeBasic * theme, const std::string& name)
{
	LoadThemeBackground(theme->Background, name);
	LoadThemeTexts(theme->Text, name + "Text");
	LoadThemeStatics(theme->Static, name + "Static");
	LoadThemeButtonCollections(theme->ButtonCollection, name + "ButtonCollection");

	_lastThemeBasic = theme;
}

void Themes::LoadThemeBackground(ThemeBackground& themeBackground, const std::string& name)
{
	const std::string tmp = name + "Background";
	const char * section = tmp.c_str();

	themeBackground.BGType  = LOOKUP_ENUM_VALUE(BackgroundType, section, "Type", BackgroundType::Auto);
	themeBackground.Tex     = ini.GetValue(section, "Tex", "");
	themeBackground.Color.R = (float) ini.GetDoubleValue(section, "ColR", 1.0);
	themeBackground.Color.G = (float) ini.GetDoubleValue(section, "ColG", 1.0);
	themeBackground.Color.B = (float) ini.GetDoubleValue(section, "ColB", 1.0);
	themeBackground.Alpha   = (float) ini.GetDoubleValue(section, "Alpha", 1.0);
}

void Themes::LoadThemeText(ThemeText& themeText, const std::string& name)
{
	const char * section = name.c_str();

	themeText.X = ini.GetLongValue(section, "X");
	themeText.Y = ini.GetLongValue(section, "Y");
	themeText.W = ini.GetLongValue(section, "W");
	themeText.Z = (float) ini.GetDoubleValue(section, "Z");

	themeText.ColRGB.R 
		= (float) ini.GetDoubleValue(section, "ColR");

	themeText.ColRGB.G 
		= (float) ini.GetDoubleValue(section, "ColG");

	themeText.ColRGB.B
		= (float) ini.GetDoubleValue(section, "ColB");

	themeText.Font = ini.GetLongValue(section, "Font", ftNormal); 
	themeText.Size = ini.GetLongValue(section, "Size");
	themeText.Align = ini.GetLongValue(section, "Align");

	themeText.Text = __T(ini.GetValue(section, "Text", ""));
	themeText.Color = ini.GetValue(section, "Color", "");

	// Reflection
	themeText.Reflection = ini.GetBoolValue(section, "Reflection", false);
	themeText.ReflectionSpacing
		= (float) ini.GetDoubleValue(section, "ReflectionSpacing", 15.0);

	LoadColor(themeText.ColRGB, themeText.Color);
}

void Themes::LoadThemeTexts(AThemeText& themeTextCollection, const std::string& name)
{
	int sectionNo = 1;
	std::string tempName;
	themeTextCollection.clear();
	while (ini.GetSectionSize((tempName 
		= (name + boost::lexical_cast<std::string>(sectionNo))).c_str()) > 0)
	{
		themeTextCollection.push_back(ThemeText());
		LoadThemeText(themeTextCollection[sectionNo - 1], tempName);
		++sectionNo;
	}
}

void Themes::LoadThemeStatic(ThemeStatic& themeStatic, const std::string& name)
{
	const char * section = name.c_str();

	themeStatic.Tex = ini.GetValue(section, "Tex", "");

	themeStatic.X = ini.GetLongValue(section, "X");
	themeStatic.Y = ini.GetLongValue(section, "Y");
	themeStatic.Z = (float) ini.GetDoubleValue(section, "Z");

	themeStatic.W = ini.GetLongValue(section, "W");
	themeStatic.H = ini.GetLongValue(section, "H");

	themeStatic.Type = LOOKUP_ENUM_VALUE(TextureType, section, "Type", TextureType::Plain);
	themeStatic.Color = ini.GetValue(section, "Color", "");

	// Reflection
	themeStatic.Reflection = ini.GetBoolValue(section, "Reflection", false);
	themeStatic.ReflectionSpacing
		= (float) ini.GetDoubleValue(section, "ReflectionSpacing", 15.0);

	themeStatic.TexX1 = (float) ini.GetDoubleValue(section, "TexX1", 0.0);
	themeStatic.TexY1 = (float) ini.GetDoubleValue(section, "TexY1", 0.0);
	themeStatic.TexX2 = (float) ini.GetDoubleValue(section, "TexX1", 1.0);
	themeStatic.TexY2 = (float) ini.GetDoubleValue(section, "TexX2", 1.0);

	LoadColor(themeStatic.ColRGB, themeStatic.Color);
}

void Themes::LoadThemeStatics(AThemeStatic& themeStaticCollection, const std::string& name)
{
	int sectionNo = 1;
	std::string tempName;
	themeStaticCollection.clear();
	while (ini.GetSectionSize((tempName 
		= (name + boost::lexical_cast<std::string>(sectionNo))).c_str()) > 0)
	{
		themeStaticCollection.push_back(ThemeStatic());
		LoadThemeStatic(themeStaticCollection[sectionNo - 1], tempName);
		++sectionNo;
	}
}

void Themes::LoadThemeButton(ThemeButton& themeButton, const std::string& name, AThemeButtonCollection* themeButtonCollection /*= NULL*/)
{
	if (ini.GetSectionSize(name.c_str()) == 0)
	{
		themeButton.Visible = false;
		return;
	}

	const char * section = name.c_str();

	themeButton.Visible = ini.GetBoolValue(section, "Visible", true);

	themeButton.Tex = ini.GetValue(section, "Tex", "");
	themeButton.X = ini.GetLongValue(section, "X");
	themeButton.Y = ini.GetLongValue(section, "Y");
	themeButton.Z = (float) ini.GetDoubleValue(section, "Z");
	themeButton.W = ini.GetLongValue(section, "W");
	themeButton.H = ini.GetLongValue(section, "H");
	themeButton.Type = LOOKUP_ENUM_VALUE(TextureType, section, "Type", TextureType::Plain);

	// Reflection
	themeButton.Reflection = ini.GetBoolValue(section, "Reflection", false);
	themeButton.ReflectionSpacing
		= (float) ini.GetDoubleValue(section, "ReflectionSpacing", 15.0);

	themeButton.ColRGB.R  = (float) ini.GetDoubleValue(section, "ColR",  1.0);
	themeButton.ColRGB.G  = (float) ini.GetDoubleValue(section, "ColG",  1.0);
	themeButton.ColRGB.B  = (float) ini.GetDoubleValue(section, "ColB",  1.0);
	themeButton.Int       = (float) ini.GetDoubleValue(section, "Int",   1.0);
	themeButton.DColRGB.R = (float) ini.GetDoubleValue(section, "DColR", 1.0);
	themeButton.DColRGB.G = (float) ini.GetDoubleValue(section, "DColG", 1.0);
	themeButton.DColRGB.B = (float) ini.GetDoubleValue(section, "DColB", 1.0);
	themeButton.DInt      = (float) ini.GetDoubleValue(section, "DInt", 1.0);

	themeButton.Color = ini.GetValue(section, "Color", "");
	themeButton.DColor = ini.GetValue(section, "DColor", "");

	LoadColor(themeButton.ColRGB, themeButton.Color);
	LoadColor(themeButton.DColRGB, themeButton.DColor);

	// Fade
	themeButton.SelectH    = ini.GetLongValue(section, "SelectH", themeButton.H);
	themeButton.SelectW    = ini.GetLongValue(section, "SelectW", themeButton.W);
	themeButton.DeselectReflectionSpacing
		= (float) ini.GetDoubleValue(section, "DeSelectReflectionSpacing", themeButton.ReflectionSpacing);
	themeButton.Fade       = ini.GetBoolValue(section, "Fade", false);
	themeButton.FadeText   = ini.GetBoolValue(section, "FadeText", false);
	themeButton.FadeTex    = ini.GetValue(section, "FadeTex", "");
	themeButton.FadeTexPos = ini.GetLongValue(section, "FadeTexPos", 0);

	if (themeButton.FadeTexPos > 4)      themeButton.FadeTexPos = 4;
	else if (themeButton.FadeTexPos < 0) themeButton.FadeTexPos = 0;

	// Button collection
	size_t parent = ini.GetLongValue(section, "Parent", 0);

	AThemeButtonCollection * collection;

	// Set collection to last basic collection if no valid value
	if (themeButtonCollection == NULL)
		collection = &_lastThemeBasic->ButtonCollection;
	else
		collection = themeButtonCollection;

	if (collection == NULL
		|| parent <= 0
		|| parent > collection->size())
	{
		themeButton.Parent = 0;
	}
	else
	{
		++(*collection)[parent - 1].ChildCount;
		themeButton.Parent = parent;
	}

	// Read button texts
	int len = ini.GetLongValue(section, "Texts", 0);
	themeButton.Text.assign(len, ThemeText());
	for (int i = 1; i <= len; i++)
		LoadThemeText(themeButton.Text[i - 1], name + "Text" + boost::lexical_cast<std::string>(i));
}

void Themes::LoadThemeButtonCollection(ThemeButtonCollection& themeButtonCollection, const std::string& name)
{
	// Default child count to 0
	themeButtonCollection.ChildCount = 0;

	// Load collection style
	LoadThemeButton(themeButtonCollection.Style, name);

	// Load other attributes
	int firstChild = ini.GetLongValue(name.c_str(), "FirstChild", 0);
	if (firstChild <= 0 || firstChild >= MAX_CHILDREN)
		themeButtonCollection.FirstChild = 0;
	else
		themeButtonCollection.FirstChild = firstChild;
}

void Themes::LoadThemeButtonCollections(AThemeButtonCollection& themeButtonCollection, const std::string& name)
{
	int sectionNo = 1;
	std::string tempName;
	themeButtonCollection.clear();
	while (ini.GetSectionSize((tempName 
		= (name + boost::lexical_cast<std::string>(sectionNo))).c_str()) > 0)
	{
		themeButtonCollection.push_back(ThemeButtonCollection());
		LoadThemeButtonCollection(themeButtonCollection[sectionNo - 1], tempName);
		++sectionNo;
	}
}

void Themes::LoadThemeSelectSlide(ThemeSelectSlide& themeSelectSlide, const std::string& name)
{
	const char * section = name.c_str();

	themeSelectSlide.Text = __T(ini.GetValue(section, "Text", ""));

	themeSelectSlide.Tex = ini.GetValue(section, "Tex", "");
	themeSelectSlide.Type = LOOKUP_ENUM_VALUE(TextureType, section, "Type", TextureType::Plain);

	themeSelectSlide.TexSBG = ini.GetValue(section, "TexSBG", "");
	themeSelectSlide.TypeSBG = LOOKUP_ENUM_VALUE(TextureType, section, "TypeSBG", TextureType::Plain);

	themeSelectSlide.X = ini.GetLongValue(section, "X", 0);
	themeSelectSlide.Y = ini.GetLongValue(section, "Y", 0);
	themeSelectSlide.Z = (float) ini.GetDoubleValue(section, "Z", 0.0);
	themeSelectSlide.W = ini.GetLongValue(section, "W", 0);
	themeSelectSlide.H = ini.GetLongValue(section, "H", 0);

	themeSelectSlide.TextSize = ini.GetLongValue(section, "TextSize", 30);
	themeSelectSlide.SkipX = ini.GetLongValue(section, "SkipX", 0);
	themeSelectSlide.SBGW = ini.GetLongValue(section, "SBGW", 400);

	LoadColor(themeSelectSlide.ColRGB, ini.GetValue(section, "Color", ""));
	themeSelectSlide.Int = (float) ini.GetDoubleValue(section, "Int", 1.0);

	LoadColor(themeSelectSlide.DColRGB, ini.GetValue(section, "DColor", ""));
	themeSelectSlide.DInt = (float) ini.GetDoubleValue(section, "DInt", 1.0);

	LoadColor(themeSelectSlide.TColRGB, ini.GetValue(section, "TColor", ""));
	themeSelectSlide.TInt = (float) ini.GetDoubleValue(section, "TInt", 1.0);

	LoadColor(themeSelectSlide.TDColRGB, ini.GetValue(section, "TDColor", ""));
	themeSelectSlide.TDInt = (float) ini.GetDoubleValue(section, "TDInt", 1.0);

	LoadColor(themeSelectSlide.SBGColRGB, ini.GetValue(section, "SBGColor", ""));
	themeSelectSlide.SBGInt = (float) ini.GetDoubleValue(section, "SBGInt", 1.0);

	LoadColor(themeSelectSlide.SBGDColRGB, ini.GetValue(section, "SBGDColor", ""));
	themeSelectSlide.SBGDInt = (float) ini.GetDoubleValue(section, "SBGDInt", 1.0);

	LoadColor(themeSelectSlide.STColRGB, ini.GetValue(section, "STColor", ""));
	themeSelectSlide.STInt = (float) ini.GetDoubleValue(section, "STInt", 1.0);

	LoadColor(themeSelectSlide.STDColRGB, ini.GetValue(section, "STDColor", ""));
	themeSelectSlide.STDInt = (float) ini.GetDoubleValue(section, "STDInt", 1.0);

	themeSelectSlide.ShowArrows = ini.GetBoolValue(section, "ShowArrays", false);
	themeSelectSlide.OneItemOnly = ini.GetBoolValue(section, "OneItemOnly", false);
}

void Themes::LoadThemeEqualizer(ThemeEqualizer& themeEqualizer, const std::string& name)
{
	const char * section = name.c_str();

	themeEqualizer.Visible = ini.GetBoolValue(section, "Visible", false);
	themeEqualizer.Direction = ini.GetBoolValue(section, "Direction", false);
	themeEqualizer.Alpha = (float) ini.GetDoubleValue(section, "Alpha", 1.0);
	themeEqualizer.Space = ini.GetLongValue(section, "Space", 1);
	themeEqualizer.X = ini.GetLongValue(section, "X", 0);
	themeEqualizer.Y = ini.GetLongValue(section, "Y", 0);
	themeEqualizer.Z = (float) ini.GetDoubleValue(section, "Z", 1.0);
	themeEqualizer.W = ini.GetLongValue(section, "PieceW", 8);
	themeEqualizer.H = ini.GetLongValue(section, "PieceH", 8);
	themeEqualizer.Bands = ini.GetLongValue(section, "Bands", 5);
	themeEqualizer.Length = ini.GetLongValue(section, "Length", 12);
	themeEqualizer.Reflection = ini.GetBoolValue(section, "Reflection", false);
	themeEqualizer.ReflectionSpacing
		= (float) ini.GetDoubleValue(section, "ReflectionSpacing", 15.0);

	LoadColor(themeEqualizer.ColRGB, ini.GetValue(section, "Color", "Black"));
}

/**
 * Looks up a given theme by its name.
 *
 * @param	themeName	Name of the theme to lookup.
 *
 * @return	NULL if it fails, else a pointer to the associated ThemeEntry.
 */
ThemeEntry * Themes::LookupTheme(std::string themeName)
{
	ThemeEntryMap::const_iterator itr = _themes.find(themeName);
	return (itr == _themes.end() ? NULL : (ThemeEntry *)&itr->second);
}

/**
 * Looks up a given theme by its name.
 * If the given theme was not found, attempt to fallback to the
 * specified default them.
 *
 * @param	themeName   	Name of the theme to lookup.
 * @param	defaultTheme	The theme to default to, in the event
 * 							the initial theme wasn't found.
 *
 * @return	NULL if it fails, else a pointer to the associated ThemeEntry.
 */
ThemeEntry * Themes::LookupThemeDefault(std::string themeName, std::string defaultTheme)
{
	ThemeEntry * result = LookupTheme(themeName);
	if (result != NULL)
		return result;

	return LookupTheme(defaultTheme);
}

Themes::~Themes()
{
	#define FREE_THEME(name) delete name

	FREE_THEME(Loading);
	FREE_THEME(Main);
	FREE_THEME(Name);
	FREE_THEME(Level);
	FREE_THEME(Song);
	FREE_THEME(Sing);
	FREE_THEME(LyricBar);
	FREE_THEME(Score);
	FREE_THEME(Top5);
	FREE_THEME(Options);
	FREE_THEME(OptionsGame);
	FREE_THEME(OptionsGraphics);
	FREE_THEME(OptionsSound);
	FREE_THEME(OptionsLyrics);
	FREE_THEME(OptionsThemes);
	FREE_THEME(OptionsRecord);
	FREE_THEME(OptionsAdvanced);
	FREE_THEME(Edit);
	FREE_THEME(ErrorPopup);
	FREE_THEME(CheckPopup);
	FREE_THEME(SongMenu);
	FREE_THEME(SongJumpTo);
	FREE_THEME(PartyNewRound);
	FREE_THEME(PartyWin);
	FREE_THEME(PartyScore);
	FREE_THEME(PartyOptions);
	FREE_THEME(PartyPlayer);
	FREE_THEME(PartyRounds);
	FREE_THEME(StatMain);
	FREE_THEME(StatDetail);
	FREE_THEME(Playlist);

	#undef FREE_THEME
}
