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
		sLog.Status(_T("Themes::LoadList"), _T("Searching for themes in directory: %s"),
			ThemePath.native().c_str());

		// Clear the existing themes list.
		_themes.clear();

		for (directory_iterator itr(ThemePath); itr != end; ++itr)
		{
			const path& p = itr->path();
			if (is_directory(p)
				|| !p.has_extension()
				|| p.extension() != _T(".ini"))
				continue;

			sLog.Status(_T("Themes::LoadList"), _T("Found theme %s"),
				p.stem().native().c_str());

			LoadHeader(p);
		}
	}
	catch (filesystem_error)
	{
		sLog.Critical(_T("Themes::LoadList"), _T("Could not access themes directory (%s)"), 
			ThemePath.native().c_str());
	}
}

void Themes::LoadHeader(const path& iniFile)
{
	CSimpleIni ini(true);
	SI_Error result = ini.LoadFile(iniFile.native().c_str());
	if (result != SI_OK)
	{
		return sLog.Warn(_T("Themes::LoadHeader"), _T("Failed to load INI (%s)"),
			iniFile.native().c_str());
	}

	const TCHAR * section = _T("Theme");

	ThemeEntry theme;
	tstring themeVersion;
	const TCHAR * creator;

	theme.Path = iniFile.branch_path();
	theme.FileName = iniFile;
	theme.Name = ini.GetValue(section, _T("Name"), _T("(no name)"));
	themeVersion = ini.GetValue(section, _T("US_Version"), _T("no version"));

	// Attempt to lookup theme's creator.
	// NOTE: Current USDX loads "Creator", but older themes use "Author".
	// Attempt to fallback to "Author" if "Creator" isn't found.
	creator				= ini.GetValue(section, _T("Creator"));
	if (creator == NULL)
		creator			= ini.GetValue(section, _T("Author"), _T(""));

	theme.Creator		= creator;

	if (themeVersion != USDX_THEME_VERSION)
	{
		return sLog.Warn(_T("Theme::LoadHeader"), _T("Wrong version (%s) for theme %s."),
			themeVersion.c_str(), theme.Name.c_str());
	}

	theme.DefaultSkin = sSkins.LookupSkinForTheme(theme.Name);
	if (theme.DefaultSkin == NULL)
	{
		return sLog.Warn(_T("Theme::LoadHeader"), _T("Skin does not exist for theme %s."),
			theme.Name.c_str());
	}

	_themes.insert(std::make_pair(theme.Name, theme));
}

void Themes::LoadTheme(ThemeEntry * theme, eColor color)
{
	if (theme == NULL)
		sLog.Critical(_T("Theme not set."));

	sLog.Status(_T("Themes::LoadTheme"), _T("Loading: %s"), 
		theme->FileName.native().c_str());

	SI_Error result = ini.LoadFile(theme->FileName.c_str());
	if (result != SI_OK)
		sLog.Critical(_T("Unable to load theme config file."));

	CreateThemeObjects();

	sSkins.LoadSkin(sIni.Skin, color);

	LoadColors();

	// Loading~
	LoadThemeBasic(Loading, _T("Loading"));
	LoadThemeText(Loading->TextLoading, _T("LoadingTextLoading"));
	LoadThemeStatic(Loading->StaticAnimation, _T("LoadingStaticAnimation"));

	// Main~
	LoadThemeBasic(Main, _T("Main"));
	LoadThemeText(Main->TextDescription, _T("MainTextDescription"));
	LoadThemeText(Main->TextDescriptionLong, _T("MainTextDescriptionLong"));
	LoadThemeButton(Main->ButtonSolo, _T("MainButtonSolo"));
	LoadThemeButton(Main->ButtonMulti, _T("MainButtonMulti"));
	LoadThemeButton(Main->ButtonStats, _T("MainButtonStats"));
	LoadThemeButton(Main->ButtonEditor, _T("MainButtonEditor"));
	LoadThemeButton(Main->ButtonOptions, _T("MainButtonOptions"));
	LoadThemeButton(Main->ButtonExit, _T("MainButtonExit"));

	// Main description translation
	Main->Description[0]     = sLanguage.Translate(_T("SING_SING"));
	Main->DescriptionLong[0] = sLanguage.Translate(_T("SING_SING_DESC"));
	Main->Description[1]     = sLanguage.Translate(_T("SING_MULTI"));
	Main->DescriptionLong[1] = sLanguage.Translate(_T("SING_MULTI_DESC"));
	Main->Description[2]     = sLanguage.Translate(_T("SING_STATS"));
	Main->DescriptionLong[2] = sLanguage.Translate(_T("SING_STATS_DESC"));
	Main->Description[3]     = sLanguage.Translate(_T("SING_EDITOR"));
	Main->DescriptionLong[3] = sLanguage.Translate(_T("SING_EDITOR_DESC"));
	Main->Description[4]     = sLanguage.Translate(_T("SING_GAME_OPTIONS"));
	Main->DescriptionLong[4] = sLanguage.Translate(_T("SING_GAME_OPTIONS_DESC"));
	Main->Description[5]     = sLanguage.Translate(_T("SING_EXIT"));
	Main->DescriptionLong[5] = sLanguage.Translate(_T("SING_EXIT_DESC"));

	// Active description
	Main->TextDescription.Text     = Main->Description[0];
	Main->TextDescriptionLong.Text = Main->DescriptionLong[0];

	// Name~
	LoadThemeBasic(Name, _T("Name"));

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		TCHAR buff[20];
		_sntprintf(buff, 20, _T("NameButtonPlayer%d"), i + 1);
		LoadThemeButton(Name->ButtonPlayer[i], buff);
	}
	
	// Level~
	LoadThemeBasic(Level, _T("Level"));
	LoadThemeButton(Level->ButtonEasy,   _T("LevelButtonEasy"));
	LoadThemeButton(Level->ButtonMedium, _T("LevelButtonMedium"));
	LoadThemeButton(Level->ButtonHard,   _T("LevelButtonHard"));

	// Song~
	LoadThemeBasic(Song, _T("Song"));
	LoadThemeText(Song->TextArtist,   _T("SongTextArtist"));
	LoadThemeText(Song->TextTitle,    _T("SongTextTitle"));
	LoadThemeText(Song->TextNumber,   _T("SongTextNumber"));

	// Video icon
	LoadThemeStatic(Song->VideoIcon,  _T("SongVideoIcon"));

	// Medley icons
	LoadThemeStatic(Song->MedleyIcon, _T("SongMedleyIcon"));
	LoadThemeStatic(Song->CalculatedMedleyIcon, _T("SongCalculatedMedleyIcon"));

	// Show cat in top-left
	LoadThemeStatic(Song->StaticCat,  _T("SongStaticCat"));
	LoadThemeText(Song->TextCat,      _T("SongTextCat"));

	// Load cover pos & size from theme
	Song->Cover.X           = ini.GetLongValue(_T("SongCover"), _T("X"), 300);
	Song->Cover.Y           = ini.GetLongValue(_T("SongCover"), _T("Y"), 190);
	Song->Cover.W           = ini.GetLongValue(_T("SongCover"), _T("W"), 300);
	Song->Cover.H           = ini.GetLongValue(_T("SongCover"), _T("H"), 200);
	Song->Cover.Style       = ini.GetLongValue(_T("SongCover"), _T("Style"), 4);
	Song->Cover.Reflections = ini.GetBoolValue(_T("SongCover"), _T("Reflections"), false);

	// Equalizer
	LoadThemeEqualizer(Song->Equalizer, _T("SongEqualizer"));

	// Party & non-party specific statics & texts
	LoadThemeStatics(Song->StaticParty, _T("SongStaticParty"));
	LoadThemeTexts(Song->TextParty, _T("SongTextParty"));
	LoadThemeStatics(Song->StaticNonParty, _T("SongStaticNonParty"));
	LoadThemeTexts(Song->TextNonParty, _T("SongTextNonParty"));

	// Party mode
	for (int teamNo = 0; teamNo < MAX_TEAMS; teamNo++)
	{
		TCHAR buff[40];

		for (int jokerNo = 0; jokerNo < MAX_JOKERS; jokerNo++)
		{
			_sntprintf(buff, 40, _T("StaticTeam%dJoker%d"), teamNo + 1, jokerNo + 1);
			LoadThemeStatic(Song->StaticTeamJoker[teamNo][jokerNo], buff);
		}
	}

	// Lyric bar
	LyricBar->UpperX = ini.GetLongValue(_T("SingLyricsUpperBar"), _T("X"), 0);
	LyricBar->UpperW = ini.GetLongValue(_T("SingLyricsUpperBar"), _T("W"), 0);
	LyricBar->UpperY = ini.GetLongValue(_T("SingLyricsUpperBar"), _T("Y"), 0);
	LyricBar->UpperH = ini.GetLongValue(_T("SingLyricsUpperBar"), _T("H"), 0);
	LyricBar->IndicatorYOffset = ini.GetLongValue(_T("SingLyricsUpperBar"), _T("IndicatorYOffset"), 0);
	LyricBar->LowerX = ini.GetLongValue(_T("SingLyricsLowerBar"), _T("X"), 0);
	LyricBar->LowerW = ini.GetLongValue(_T("SingLyricsLowerBar"), _T("W"), 0);
	LyricBar->LowerY = ini.GetLongValue(_T("SingLyricsLowerBar"), _T("Y"), 0);
	LyricBar->LowerH = ini.GetLongValue(_T("SingLyricsLowerBar"), _T("H"), 0);

	// Sing~
	LoadThemeBasic(Sing, _T("Sing"));

	// Time bar
	LoadThemeStatic(Sing->StaticTimeProgress, _T("SingTimeProgress"));
	LoadThemeText(Sing->TextTimeText, _T("SingTimeText"));

	// Moveable singbar
	LoadThemeStatic(Sing->StaticP1SingBar, _T("SingP1SingBar"));
	LoadThemeStatic(Sing->StaticP1TwoPSingBar, _T("SingP1TwoPSingBar"));
	LoadThemeStatic(Sing->StaticP1ThreePSingBar, _T("SingP1ThreePSingBar"));
	LoadThemeStatic(Sing->StaticP2RSingBar, _T("SingP2RSingBar"));
	LoadThemeStatic(Sing->StaticP2MSingBar, _T("SingP2MSingBar"));
	LoadThemeStatic(Sing->StaticP3SingBar, _T("SingP3SingBar"));

	LoadThemeStatic(Sing->StaticP1, _T("SingP1Static"));
	LoadThemeText(Sing->TextP1, _T("SingP1Text"));
	LoadThemeStatic(Sing->StaticP1ScoreBG, _T("SingP1Static2"));
	LoadThemeText(Sing->TextP1Score, _T("SingP1TextScore"));

	// This one is shown in 2/4 player mode if it exists.
	// If not, the player equivalents are used.
	if (ini.GetSectionSize(_T("SingP1TwoPTextScore")) > 0)
	{
		LoadThemeStatic(Sing->StaticP1TwoP, _T("SingP1TwoPStatic"));
		LoadThemeText(Sing->TextP1TwoP, _T("SingP1TwoPText"));
		LoadThemeStatic(Sing->StaticP1TwoPScoreBG, _T("SingP1TwoPStatic2"));
		LoadThemeText(Sing->TextP1TwoPScore, _T("SingP1TwoPTextScore"));
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
	if (ini.GetSectionSize(_T("SingP1ThreePTextScore")) > 0)
	{
		LoadThemeStatic(Sing->StaticP1ThreeP, _T("SingP1ThreePStatic"));
		LoadThemeText(Sing->TextP1ThreeP, _T("SingP1ThreeText"));
		LoadThemeStatic(Sing->StaticP1ThreePScoreBG, _T("SingP1ThreePStatic2"));
		LoadThemeText(Sing->TextP1ThreePScore, _T("SingP1ThreePTextScore"));
	}
	else
	{
		Sing->StaticP1ThreeP = Sing->StaticP1;
		Sing->TextP1ThreeP = Sing->TextP1;
		Sing->StaticP1ThreePScoreBG = Sing->StaticP1ScoreBG;
		Sing->TextP1ThreePScore = Sing->TextP1Score;
	}

	LoadThemeStatic(Sing->StaticP2R, _T("SingP2RStatic"));
	LoadThemeText(Sing->TextP2R, _T("SingP2RText"));
	LoadThemeStatic(Sing->StaticP2RScoreBG, _T("SingP2RStatic2"));
	LoadThemeText(Sing->TextP2RScore, _T("SingP2RTextScore"));

	LoadThemeStatic(Sing->StaticP2M, _T("SingP2MStatic"));
	LoadThemeText(Sing->TextP2M, _T("SingP2MText"));
	LoadThemeStatic(Sing->StaticP2MScoreBG, _T("SingP2MStatic2"));
	LoadThemeText(Sing->TextP2MScore, _T("SingP2MTextScore"));

	LoadThemeStatic(Sing->StaticP3R, _T("SingP3RStatic"));
	LoadThemeText(Sing->TextP3R, _T("SingP3RText"));
	LoadThemeStatic(Sing->StaticP3RScoreBG, _T("SingP3RStatic2"));
	LoadThemeText(Sing->TextP3RScore, _T("SingP3RTextScore"));

	LoadThemeStatic(Sing->StaticSongName, _T("SingSongNameStatic"));
	LoadThemeText(Sing->TextSongName, _T("SingSongNameText"));

	// Line bonus texts
	Sing->LineBonusText[0] = sLanguage.Translate(_T("POPUP_AWFUL"));
	Sing->LineBonusText[1] = Sing->LineBonusText[0];
	Sing->LineBonusText[2] = sLanguage.Translate(_T("POPUP_POOR"));
	Sing->LineBonusText[3] = sLanguage.Translate(_T("POPUP_BAD"));
	Sing->LineBonusText[4] = sLanguage.Translate(_T("POPUP_NOTBAD"));
	Sing->LineBonusText[5] = sLanguage.Translate(_T("POPUP_GOOD"));
	Sing->LineBonusText[6] = sLanguage.Translate(_T("POPUP_GREAT"));
	Sing->LineBonusText[7] = sLanguage.Translate(_T("POPUP_AWESOME"));
	Sing->LineBonusText[8] = sLanguage.Translate(_T("POPUP_PERFECT"));

	// Pause popup
	LoadThemeStatic(Sing->PausePopup, _T("PausePopUpStatic"));

	// Score~
	LoadThemeBasic(Score, _T("Score"));
	LoadThemeText(Score->TextArtist,      _T("ScoreTextArtist"));
	LoadThemeText(Score->TextTitle,       _T("ScoreTextTitle"));
	LoadThemeText(Score->TextArtistTitle, _T("ScoreTextArtistTitle"));

	#define F(format, ...) \
		_sntprintf(buff, sizeof(buff) / sizeof(buff[0]), format, __VA_ARGS__)

	for (int playerNo = 1; playerNo <= MAX_PLAYERS; playerNo++)
	{
		TCHAR buff[40];
		int playerID = playerNo - 1;

		#define P(format) \
			F(format, playerNo)

		P(_T("ScorePlayer%dStatic"));         LoadThemeStatics(Score->PlayerStatic[playerID], buff);
		P(_T("ScorePlayer%dText"));           LoadThemeTexts(Score->PlayerTexts[playerID], buff);

		P(_T("ScoreTextName%d"));             LoadThemeText(Score->TextName[playerID], buff);
		P(_T("ScoreTextScore%d"));            LoadThemeText(Score->TextScore[playerID], buff);
		P(_T("ScoreTextNotes%d"));            LoadThemeText(Score->TextNotes[playerID], buff);
		P(_T("ScoreTextNotesScore%d"));       LoadThemeText(Score->TextNotesScore[playerID], buff);
		P(_T("ScoreTextLineBonus%d"));        LoadThemeText(Score->TextLineBonus[playerID], buff);
		P(_T("ScoreTextLineBonusScore%d"));   LoadThemeText(Score->TextLineBonusScore[playerID], buff);
		P(_T("ScoreTextGoldenNotes%d"));      LoadThemeText(Score->TextGoldenNotes[playerID], buff);
		P(_T("ScoreTextGoldenNotesScore%d")); LoadThemeText(Score->TextGoldenNotesScore[playerID], buff);
		P(_T("ScoreTextTotal%d"));            LoadThemeText(Score->TextTotal[playerID], buff);
		P(_T("ScoreTextTotalScore%d"));       LoadThemeText(Score->TextTotalScore[playerID], buff);

		P(_T("ScoreStaticBoxLightest%d"));    LoadThemeStatic(Score->StaticBoxLightest[playerID], buff);
		P(_T("ScoreStaticBoxLight%d"));       LoadThemeStatic(Score->StaticBoxLight[playerID], buff);
		P(_T("ScoreStaticBoxDark%d"));        LoadThemeStatic(Score->StaticBoxDark[playerID], buff);

		P(_T("ScoreStaticBackLevel%d"));      LoadThemeStatic(Score->StaticBackLevel[playerID], buff);
		P(_T("ScoreStaticBackLevelRound%d")); LoadThemeStatic(Score->StaticBackLevelRound[playerID], buff);
		P(_T("ScoreStaticLevel%d"));          LoadThemeStatic(Score->StaticLevel[playerID], buff);
		P(_T("ScoreStaticLevelRound%d"));     LoadThemeStatic(Score->StaticLevelRound[playerID], buff);
		P(_T("ScoreStaticPlayerIdBox%d"));    LoadThemeStatic(Score->StaticPlayerIdBox[playerID], buff);

		P(_T("ScoreStaticRatingPicture%d"));  LoadThemeStatic(Score->StaticRatings[playerID], buff);

		#undef P
	}

	LoadThemeStatic(Score->StaticNavigate, _T("ScoreStaticNavigate"));
	LoadThemeText(Score->TextNavigate, _T("ScoreTextNavigate"));
	
	// Top5~
	LoadThemeBasic(Top5, _T("Top5"));
	LoadThemeText(Top5->TextLevel, _T("Top5TextLevel"));
	LoadThemeText(Top5->TextArtistTitle, _T("Top5TextArtistTitle"));
	LoadThemeStatics(Top5->StaticNumber, _T("Top5StaticNumber"));
	LoadThemeTexts(Top5->TextNumber, _T("Top5TextNumber"));
	LoadThemeTexts(Top5->TextName, _T("Top5TextName"));
	LoadThemeTexts(Top5->TextScore, _T("Top5TextScore"));
	LoadThemeTexts(Top5->TextDate, _T("Top5TextDate"));

	// Options~
	LoadThemeBasic(Options, _T("Options"));
	LoadThemeButton(Options->ButtonGame, _T("OptionsButtonGame"));
	LoadThemeButton(Options->ButtonGraphics, _T("OptionsButtonGraphics"));
	LoadThemeButton(Options->ButtonSound, _T("OptionsButtonSound"));
	LoadThemeButton(Options->ButtonLyrics, _T("OptionsButtonLyrics"));
	LoadThemeButton(Options->ButtonThemes, _T("OptionsButtonThemes"));
	LoadThemeButton(Options->ButtonRecord, _T("OptionsButtonRecord"));
	LoadThemeButton(Options->ButtonAdvanced, _T("OptionsButtonAdvanced"));
	LoadThemeButton(Options->ButtonExit, _T("OptionsButtonExit"));
	LoadThemeText(Options->TextDescription, _T("OptionsTextDescription"));

	Options->Description[0] = sLanguage.Translate(_T("SING_OPTIONS_GAME_DESC"));
	Options->Description[1] = sLanguage.Translate(_T("SING_OPTIONS_GRAPHICS_DESC"));
	Options->Description[2] = sLanguage.Translate(_T("SING_OPTIONS_SOUND_DESC"));
	Options->Description[3] = sLanguage.Translate(_T("SING_OPTIONS_LYRICS_DESC"));
	Options->Description[4] = sLanguage.Translate(_T("SING_OPTIONS_THEMES_DESC"));
	Options->Description[5] = sLanguage.Translate(_T("SING_OPTIONS_RECORD_DESC"));
	Options->Description[6] = sLanguage.Translate(_T("SING_OPTIONS_ADVANCED_DESC"));
	Options->Description[7] = sLanguage.Translate(_T("SING_OPTIONS_EXIT"));

	// Set active description
	Options->TextDescription.Text = Options->Description[0];

	// Options: game~
	LoadThemeBasic(OptionsGame, _T("OptionsGame"));
	LoadThemeSelectSlide(OptionsGame->SelectPlayers, _T("OptionsGameSelectPlayers"));
	LoadThemeSelectSlide(OptionsGame->SelectDifficulty, _T("OptionsGameSelectDifficulty"));
	LoadThemeSelectSlide(OptionsGame->SelectLanguage, _T("OptionsGameSelectSlideLanguage"));
	LoadThemeSelectSlide(OptionsGame->SelectTabs, _T("OptionsGameSelectTabs"));
	LoadThemeSelectSlide(OptionsGame->SelectSorting, _T("OptionsGameSelectSlideSorting"));
	LoadThemeSelectSlide(OptionsGame->SelectDebug, _T("OptionsGameSelectDebug"));
	LoadThemeButton(OptionsGame->ButtonExit, _T("OptionsGameButtonExit"));

	// Options: graphics~
	LoadThemeBasic(OptionsGraphics, _T("OptionsGraphics"));
	LoadThemeSelectSlide(OptionsGraphics->SelectFullscreen, _T("OptionsGraphicsSelectFullscreen"));
	LoadThemeSelectSlide(OptionsGraphics->SelectResolution, _T("OptionsGraphicsSelectResolution"));
	LoadThemeSelectSlide(OptionsGraphics->SelectDepth, _T("OptionsGraphicsSelectDepth"));
	LoadThemeSelectSlide(OptionsGraphics->SelectVisualizer, _T("OptionsGraphicsSelectVisualizer"));
	LoadThemeSelectSlide(OptionsGraphics->SelectOscilloscope, _T("OptionsGraphicsSelectOscilloscope"));
	LoadThemeSelectSlide(OptionsGraphics->SelectLineBonus, _T("OptionsGraphicsSelectLineBonus"));
	LoadThemeSelectSlide(OptionsGraphics->SelectMovieSize, _T("OptionsGraphicsSelectMovieSize"));
	LoadThemeButton(OptionsGraphics->ButtonExit, _T("OptionsGraphicsButtonExit"));

	// Options: sound~
	LoadThemeBasic(OptionsSound, _T("OptionsSound"));
	LoadThemeSelectSlide(OptionsSound->SelectBackgroundMusic, _T("OptionsSoundSelectBackgroundMusic"));
	LoadThemeSelectSlide(OptionsSound->SelectMicBoost, _T("OptionsSoundSelectMicBoost"));
	LoadThemeSelectSlide(OptionsSound->SelectClickAssist, _T("OptionsSoundSelectClickAssist"));
	LoadThemeSelectSlide(OptionsSound->SelectBeatClick, _T("OptionsSoundSelectBeatClick"));
	LoadThemeSelectSlide(OptionsSound->SelectThreshold, _T("OptionsSoundSelectThreshold"));
	LoadThemeSelectSlide(OptionsSound->SelectSlidePreviewVolume, _T("OptionsSoundSelectSlidePreviewVolume"));
	LoadThemeSelectSlide(OptionsSound->SelectSlidePreviewFading, _T("OptionsSoundSelectSlidePreviewFading"));
	LoadThemeSelectSlide(OptionsSound->SelectSlideVoicePassthrough, _T("OptionsSoundSelectVoicePassthrough"));
	LoadThemeButton(OptionsSound->ButtonExit, _T("OptionsSoundButtonExit"));

	// Options: lyrics~
	LoadThemeBasic(OptionsLyrics, _T("OptionsLyrics"));
	LoadThemeSelectSlide(OptionsLyrics->SelectLyricsFont, _T("OptionsLyricsSelectLyricsFont"));
	LoadThemeSelectSlide(OptionsLyrics->SelectLyricsEffect, _T("OptionsLyricsSelectLyricsEffect"));
	LoadThemeSelectSlide(OptionsLyrics->SelectNoteLines, _T("OptionsLyricsSelectNoteLines"));
	LoadThemeButton(OptionsLyrics->ButtonExit, _T("OptionsLyricsButtonExit"));
	
	// Options: themes~
	LoadThemeBasic(OptionsThemes, _T("OptionsThemes"));
	LoadThemeSelectSlide(OptionsThemes->SelectTheme, _T("OptionsThemesSelectTheme"));
	LoadThemeSelectSlide(OptionsThemes->SelectSkin, _T("OptionsThemesSelectSkin"));
	LoadThemeSelectSlide(OptionsThemes->SelectColor, _T("OptionsThemesSelectColor"));
	LoadThemeButton(OptionsThemes->ButtonExit, _T("OptionsThemesButtonExit"));

	// Options: record~
	LoadThemeBasic(OptionsRecord, _T("OptionsRecord"));
	LoadThemeSelectSlide(OptionsRecord->SelectSlideCard, _T("OptionsRecordSelectSlideCard"));
	LoadThemeSelectSlide(OptionsRecord->SelectSlideInput, _T("OptionsRecordSelectSlideInput"));
	LoadThemeSelectSlide(OptionsRecord->SelectSlideChannel, _T("OptionsRecordSelectSlideChannel"));
	LoadThemeButton(OptionsRecord->ButtonExit, _T("OptionsRecordButtonExit"));

	// Options: advanced~
	LoadThemeBasic(OptionsAdvanced, _T("OptionsAdvanced"));
	LoadThemeSelectSlide(OptionsAdvanced->SelectLoadAnimation, _T("OptionsAdvancedSelectLoadAnimation"));
	LoadThemeSelectSlide(OptionsAdvanced->SelectScreenFade, _T("OptionsAdvancedSelectScreenFade"));
	LoadThemeSelectSlide(OptionsAdvanced->SelectEffectSing, _T("OptionsAdvancedSelectEffectSing"));
	LoadThemeSelectSlide(OptionsAdvanced->SelectLineBonus, _T("OptionsAdvancedSelectLineBonus"));
	LoadThemeSelectSlide(OptionsAdvanced->SelectOnSongClick, _T("OptionsAdvancedSelectOnSongClick"));
	LoadThemeSelectSlide(OptionsAdvanced->SelectAskBeforeDel, _T("OptionsAdvancedSelectAskBeforeDel"));
	LoadThemeSelectSlide(OptionsAdvanced->SelectPartyPopup, _T("OptionsAdvancedSelectPartyPopup"));
	LoadThemeButton(OptionsAdvanced->ButtonExit, _T("OptionsAdvancedButtonExit"));

	// Edit menu~
	LoadThemeBasic(Edit, _T("Edit"));
	LoadThemeButton(Edit->ButtonConvert, _T("EditButtonConvert"));
	LoadThemeButton(Edit->ButtonExit, _T("EditButtonExit"));
	LoadThemeText(Edit->TextDescription, _T("EditTextDescription"));

	Edit->Description[0] = sLanguage.Translate(_T("SING_EDIT_BUTTON_DESCRIPTION_CONVERT"));
	Edit->Description[1] = sLanguage.Translate(_T("SING_EDIT_BUTTON_DESCRIPTION_EXIT"));

	// Set active description
	Edit->TextDescription.Text = Edit->Description[0];

	// Error popup~
	LoadThemeBasic(ErrorPopup, _T("ErrorPopup"));
	LoadThemeButton(ErrorPopup->Button1, _T("ErrorPopupButton1"));
	LoadThemeText(ErrorPopup->TextError, _T("ErrorPopupText"));

	// Check popup~
	LoadThemeBasic(CheckPopup, _T("CheckPopup"));
	LoadThemeButton(CheckPopup->Button1, _T("CheckPopupButton1"));
	LoadThemeButton(CheckPopup->Button2, _T("CheckPopupButton2"));
	LoadThemeText(CheckPopup->TextCheck, _T("CheckPopupText"));
	
	// Song menu~
	LoadThemeBasic(SongMenu, _T("SongMenu"));
	LoadThemeButton(SongMenu->Button1, _T("SongMenuButton1"));
	LoadThemeButton(SongMenu->Button2, _T("SongMenuButton2"));
	LoadThemeButton(SongMenu->Button3, _T("SongMenuButton3"));
	LoadThemeButton(SongMenu->Button4, _T("SongMenuButton4"));
	LoadThemeSelectSlide(SongMenu->SelectSlide3, _T("SongMenuSelectSlide3"));
	LoadThemeText(SongMenu->TextMenu, _T("SongMenuTextMenu"));

	// Song JumpTo~
	LoadThemeBasic(SongJumpTo, _T("SongJumpTo"));
	LoadThemeButton(SongJumpTo->ButtonSearchText, _T("SongJumpToButtonSearchText"));
	LoadThemeSelectSlide(SongJumpTo->SelectSlideType, _T("SongJumpToSelectSlideType"));
	LoadThemeText(SongJumpTo->TextFound, _T("SongJumpToTextFound"));

	// Translations
	SongJumpTo->IType[0] = sLanguage.Translate(_T("SONG_JUMPTO_TYPE1"));
	SongJumpTo->IType[1] = sLanguage.Translate(_T("SONG_JUMPTO_TYPE2"));
	SongJumpTo->IType[2] = sLanguage.Translate(_T("SONG_JUMPTO_TYPE3"));
	SongJumpTo->SongsFound = sLanguage.Translate(_T("SONG_JUMPTO_SONGSFOUND"));
	SongJumpTo->NoSongsFound = sLanguage.Translate(_T("SONG_JUMPTO_NOSONGSFOUND"));
	SongJumpTo->CatText = sLanguage.Translate(_T("SONG_JUMPTO_CATTEXT"));

	// Party: new round~
	LoadThemeBasic(PartyNewRound, _T("PartyNewRound"));

	LoadThemeText(PartyNewRound->TextNextRound, _T("PartyNewRoundTextNextRound"));
	LoadThemeText(PartyNewRound->TextNextRoundNo, _T("PartyNewRoundTextNextRoundNo"));

	for (int roundNo = 0; roundNo < MAX_ROUNDS; roundNo++)
	{
		TCHAR buff[40];

		F(_T("PartyNewRoundTextRound%d"), roundNo + 1);
		LoadThemeText(PartyNewRound->TextRound[roundNo], buff);

		F(_T("PartyNewRoundTextWinner%d"), roundNo + 1);
		LoadThemeText(PartyNewRound->TextWinner[roundNo], buff);

		F(_T("PartyNewRoundStaticRound%d"), roundNo + 1);
		LoadThemeStatic(PartyNewRound->StaticRound[roundNo], buff);
	}

	for (int teamNo = 0; teamNo < MAX_TEAMS; teamNo++)
	{
		ThemePartyNewRound::Team& team = PartyNewRound->Teams[teamNo];
		TCHAR buff[40];

		F(_T("PartyNewRoundTextNextPlayer%d"), teamNo + 1);
		LoadThemeText(team.TextNextPlayer, buff);

		F(_T("PartyNewRoundStaticNextPlayer%d"), teamNo + 1);
		LoadThemeStatic(team.StaticNextPlayer, buff);

		F(_T("PartyNewRoundTextScoreTeam%d"), teamNo + 1);
		LoadThemeText(team.TextScore, buff);

		F(_T("PartyNewRoundTextNameTeam%d"), teamNo + 1);
		LoadThemeText(team.TextName, buff);

		F(_T("PartyNewRoundTextTeam%dPlayers"), teamNo + 1);
		LoadThemeText(team.TextPlayers, buff);

		F(_T("PartyNewRoundStaticTeam%d"), teamNo + 1);
		LoadThemeStatic(team.StaticTeam, buff);
	}

	// Party: score~
	LoadThemeBasic(PartyScore, _T("PartyScore"));
	LoadThemeText(PartyScore->TextWinner, _T("PartyScoreTextWinner"));

	for (int teamNo = 0; teamNo < MAX_TEAMS; teamNo++)
	{
		ThemePartyScore::Team& team = PartyScore->Teams[teamNo];
		TCHAR buff[40];

		F(_T("PartyScoreTextScoreTeam%d"), teamNo + 1);
		LoadThemeText(team.TextScore, buff);

		F(_T("PartyScoreTextNameTeam%d"), teamNo + 1);
		LoadThemeText(team.TextName, buff);

		F(_T("PartyScoreStaticTeam%d"), teamNo + 1);
		LoadThemeStatic(team.StaticTeam, buff);

		F(_T("PartyScoreStaticTeam%dBG"), teamNo + 1);
		LoadThemeStatic(team.StaticTeamBG, buff);

		F(_T("PartyScoreStaticTeam%dDeco"), teamNo + 1);
		LoadThemeStatic(team.StaticTeamDeco, buff);
	}

	PartyScore->DecoTextures.ChangeTextures = ini.GetBoolValue(_T("PartyScoreDecoTextures"), _T("ChangeTextures"), false);

	PartyScore->DecoTextures.Texture[0] = ini.GetValue(_T("PartyScoreDecoTextures"), _T("FirstTexture"), _T(""));
	PartyScore->DecoTextures.Type   [0] = LOOKUP_ENUM_VALUE(TextureType, _T("PartyScoreDecoTextures"), _T("FirstTyp"), TextureType::Colorized);
	PartyScore->DecoTextures.Color  [0] = ini.GetValue(_T("PartyScoreDecoTextures"), _T("FirstColor"), _T("Black"));

	PartyScore->DecoTextures.Texture[1] = ini.GetValue(_T("PartyScoreDecoTextures"), _T("SecondTexture"), _T(""));
	PartyScore->DecoTextures.Type   [1] = LOOKUP_ENUM_VALUE(TextureType, _T("PartyScoreDecoTextures"), _T("SecondTyp"), TextureType::Colorized);
	PartyScore->DecoTextures.Color  [1] = ini.GetValue(_T("PartyScoreDecoTextures"), _T("SecondColor"), _T("Black"));

	PartyScore->DecoTextures.Texture[2] = ini.GetValue(_T("PartyScoreDecoTextures"), _T("ThirdTexture"), _T(""));
	PartyScore->DecoTextures.Type   [2] = LOOKUP_ENUM_VALUE(TextureType, _T("PartyScoreDecoTextures"), _T("ThirdTyp"), TextureType::Colorized);
	PartyScore->DecoTextures.Color  [2] = ini.GetValue(_T("PartyScoreDecoTextures"), _T("ThirdColor"), _T("Black"));

	// Party: options~
	LoadThemeBasic(PartyOptions, _T("PartyOptions"));
	LoadThemeSelectSlide(PartyOptions->SelectLevel,     _T("PartyOptionsSelectLevel"));
	LoadThemeSelectSlide(PartyOptions->SelectPlaylist,  _T("PartyOptionsSelectPlaylist"));
	LoadThemeSelectSlide(PartyOptions->SelectPlaylist2, _T("PartyOptionsSelectPlaylist2"));

	// Party: player~
	LoadThemeBasic(PartyPlayer, _T("PartyPlayer"));
	LoadThemeSelectSlide(PartyPlayer->SelectTeams, _T("PartyPlayerSelectTeams"));

	for (int teamNo = 0, playerNo = 0; teamNo < MAX_TEAMS; teamNo++)
	{
		ThemePartyPlayer::Team& team = PartyPlayer->Teams[teamNo];
		TCHAR buff[40];

		F(_T("PartyPlayerTeam%dName"), teamNo + 1);
		LoadThemeButton(team.TeamName, buff);

		F(_T("PartyPlayerSelectPlayers%d"), teamNo + 1);
		LoadThemeSelectSlide(team.SelectPlayers, buff);

		for (int i = 0; i < MAX_PLAYERS_PER_TEAM; i++, playerNo++)
		{
			F(_T("PartyPlayerPlayer%dName"), playerNo + 1);
			LoadThemeButton(team.PlayerName[i], buff);
		}
	}

	// Party: rounds~
	LoadThemeBasic(PartyRounds, _T("PartyRounds"));

	LoadThemeSelectSlide(PartyRounds->SelectRoundCount, _T("PartyRoundsSelectRoundCount"));
	for (int roundNo = 0; roundNo < MAX_ROUNDS; roundNo++)
	{
		TCHAR buff[40];
		F(_T("PartyRoundsSelectRound%d"), roundNo + 1);
		LoadThemeSelectSlide(PartyRounds->SelectRound[roundNo], buff);
	}

	// Stat: main~
	LoadThemeBasic(StatMain, _T("StatMain"));
	LoadThemeButton(StatMain->ButtonScores, _T("StatMainButtonScores"));
	LoadThemeButton(StatMain->ButtonSingers, _T("StatMainButtonSingers"));
	LoadThemeButton(StatMain->ButtonSongs, _T("StatMainButtonSongs"));
	LoadThemeButton(StatMain->ButtonBands, _T("StatMainButtonBands"));
	LoadThemeButton(StatMain->ButtonExit, _T("StatMainButtonExit"));
	LoadThemeText(StatMain->TextOverview, _T("StatMainTextOverview"));

	// Stat: detail~
	LoadThemeBasic(StatDetail, _T("StatDetail"));
	LoadThemeButton(StatDetail->ButtonNext, _T("StatDetailButtonNext"));
	LoadThemeButton(StatDetail->ButtonPrev, _T("StatDetailButtonPrev"));
	LoadThemeButton(StatDetail->ButtonReverse, _T("StatDetailButtonReverse"));
	LoadThemeButton(StatDetail->ButtonExit, _T("StatDetailButtonExit"));
	LoadThemeText(StatDetail->TextDescription, _T("StatDetailTextDescription"));
	LoadThemeText(StatDetail->TextPage, _T("StatDetailTextPage"));
	LoadThemeTexts(StatDetail->TextList, _T("StatDetailTextList"));

	// Translate texts
	StatDetail->Description[0] = sLanguage.Translate(_T("STAT_DESC_SCORES"));
	StatDetail->Description[1] = sLanguage.Translate(_T("STAT_DESC_SINGERS"));
	StatDetail->Description[2] = sLanguage.Translate(_T("STAT_DESC_SONGS"));
	StatDetail->Description[3] = sLanguage.Translate(_T("STAT_DESC_BANDS"));

	StatDetail->DescriptionR[0] = sLanguage.Translate(_T("STAT_DESC_SCORES_REVERSED"));
	StatDetail->DescriptionR[1] = sLanguage.Translate(_T("STAT_DESC_SINGERS_REVERSED"));
	StatDetail->DescriptionR[2] = sLanguage.Translate(_T("STAT_DESC_SONGS_REVERSED"));
	StatDetail->DescriptionR[3] = sLanguage.Translate(_T("STAT_DESC_BANDS_REVERSED"));

	StatDetail->FormatStr[0] = sLanguage.Translate(_T("STAT_FORMAT_SCORES"));
	StatDetail->FormatStr[1] = sLanguage.Translate(_T("STAT_FORMAT_SINGERS"));
	StatDetail->FormatStr[2] = sLanguage.Translate(_T("STAT_FORMAT_SONGS"));
	StatDetail->FormatStr[3] = sLanguage.Translate(_T("STAT_FORMAT_BANDS"));

	StatDetail->PageStr = sLanguage.Translate(_T("STAT_PAGE"));

	// Playlist translations~
	Playlist->CatText = sLanguage.Translate(_T("PLAYLIST_CATTEXT"));

#undef F
}

void Themes::LoadColors()
{
	const TCHAR * section = _T("Colors");
	const CSimpleIni::TKeyVal * sectionKeys = ini.GetSection(section);

	// Clear color map
	_colors.clear();

	if (sectionKeys != NULL)
	{
		for (CSimpleIni::TKeyVal::const_iterator itr = sectionKeys->begin(); itr != sectionKeys->end(); ++itr)
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

	_colors.insert(std::make_pair(_T("ColorDark"),     skinRGB[0]));
	_colors.insert(std::make_pair(_T("ColorLight"),    skinRGB[1]));
	_colors.insert(std::make_pair(_T("ColorLightest"), skinRGB[2]));

	eColor playerColours[MAX_PLAYERS] = 
	{
		Color::Blue, Color::Red, Color::Green,
		Color::Brown, Color::Yellow, Color::Violet
	};

	for (int i = 1; i <= MAX_PLAYERS; i++)
	{
		RGB playerRGB[3];
		TCHAR name[20];

		// Dark (0), Light (1), Lightest (2)
		playerRGB[0] = GetSystemColor(playerColours[i - 1]);
		playerRGB[1].Sqrt(playerRGB[0]);
		playerRGB[2].Sqrt(playerRGB[1]);

		_sntprintf(name, 20, _T("P%dDark"), i);
		_colors.insert(std::make_pair(name, playerRGB[0]));

		_sntprintf(name, 20, _T("P%dLight"), i);
		_colors.insert(std::make_pair(name, playerRGB[1]));

		_sntprintf(name, 20, _T("P%dLightest"), i);
		_colors.insert(std::make_pair(name, playerRGB[2]));
	}
}

void Themes::LoadColor(RGB& rgb, const tstring& color)
{
	ColorMap::iterator itr = _colors.find(color);
	if (itr != _colors.end())
		rgb = itr->second;
	else
		rgb.R = rgb.G = rgb.B = 0.0f;
}

void Themes::LoadColor(RGB& rgb, const TCHAR * fmt, ...)
{
	TCHAR buffer[1024];
	tstring color;
	va_list args;
	va_start(args, fmt);
	_vsntprintf(buffer, 1024, fmt, args);
	va_end(args);

	color = buffer;
	LoadColor(rgb, color);
}

void Themes::LoadThemeBasic(ThemeBasic * theme, const tstring& name)
{
	LoadThemeBackground(theme->Background, name);
	LoadThemeTexts(theme->Text, name + _T("Text"));
	LoadThemeStatics(theme->Static, name + _T("Static"));
	LoadThemeButtonCollections(theme->ButtonCollection, name + _T("ButtonCollection"));

	_lastThemeBasic = theme;
}

void Themes::LoadThemeBackground(ThemeBackground& themeBackground, const tstring& name)
{
	const tstring tmp = name + _T("Background");
	const TCHAR * section = tmp.c_str();

	themeBackground.BGType  = LOOKUP_ENUM_VALUE(BackgroundType, section, _T("Type"), BackgroundType::Auto);
	themeBackground.Tex     = ini.GetValue(section, _T("Tex"), _T(""));
	themeBackground.Color.R = (float) ini.GetDoubleValue(section, _T("ColR"), 1.0);
	themeBackground.Color.G = (float) ini.GetDoubleValue(section, _T("ColG"), 1.0);
	themeBackground.Color.B = (float) ini.GetDoubleValue(section, _T("ColB"), 1.0);
	themeBackground.Alpha   = (float) ini.GetDoubleValue(section, _T("Alpha"), 1.0);
}

void Themes::LoadThemeText(ThemeText& themeText, const tstring& name)
{
	const TCHAR * section = name.c_str();

	themeText.X = ini.GetLongValue(section, _T("X"));
	themeText.Y = ini.GetLongValue(section, _T("Y"));
	themeText.W = ini.GetLongValue(section, _T("W"));
	themeText.Z = (float) ini.GetDoubleValue(section, _T("Z"));

	themeText.ColRGB.R 
		= (float) ini.GetDoubleValue(section, _T("ColR"));

	themeText.ColRGB.G 
		= (float) ini.GetDoubleValue(section, _T("ColG"));

	themeText.ColRGB.B
		= (float) ini.GetDoubleValue(section, _T("ColB"));

	themeText.Font = ini.GetLongValue(section, _T("Font"), ftNormal); 
	themeText.Size = ini.GetLongValue(section, _T("Size"));
	themeText.Align = ini.GetLongValue(section, _T("Align"));

	themeText.Text = sLanguage.Translate(ini.GetValue(section, _T("Text"), _T("")));
	themeText.Color = ini.GetValue(section, _T("Color"), _T(""));

	// Reflection
	themeText.Reflection = ini.GetBoolValue(section, _T("Reflection"), false);
	themeText.ReflectionSpacing
		= (float) ini.GetDoubleValue(section, _T("ReflectionSpacing"), 15.0);

	LoadColor(themeText.ColRGB, themeText.Color);
}

void Themes::LoadThemeTexts(AThemeText& themeTextCollection, const tstring& name)
{
	int sectionNo = 1;
	tstring tempName;
	themeTextCollection.clear();
	while (ini.GetSectionSize((tempName 
		= (name + boost::lexical_cast<tstring>(sectionNo))).c_str()) > 0)
	{
		themeTextCollection.push_back(ThemeText());
		LoadThemeText(themeTextCollection[sectionNo - 1], tempName);
		++sectionNo;
	}
}

void Themes::LoadThemeStatic(ThemeStatic& themeStatic, const tstring& name)
{
	const TCHAR * section = name.c_str();

	themeStatic.Tex = ini.GetValue(section, _T("Tex"), _T(""));

	themeStatic.X = ini.GetLongValue(section, _T("X"));
	themeStatic.Y = ini.GetLongValue(section, _T("Y"));
	themeStatic.Z = (float) ini.GetDoubleValue(section, _T("Z"));

	themeStatic.W = ini.GetLongValue(section, _T("W"));
	themeStatic.H = ini.GetLongValue(section, _T("H"));

	themeStatic.Type = LOOKUP_ENUM_VALUE(TextureType, section, _T("Type"), TextureType::Plain);
	themeStatic.Color = ini.GetValue(section, _T("Color"), _T(""));

	// Reflection
	themeStatic.Reflection = ini.GetBoolValue(section, _T("Reflection"), false);
	themeStatic.ReflectionSpacing
		= (float) ini.GetDoubleValue(section, _T("ReflectionSpacing"), 15.0);

	themeStatic.TexX1 = (float) ini.GetDoubleValue(section, _T("TexX1"), 0.0);
	themeStatic.TexY1 = (float) ini.GetDoubleValue(section, _T("TexY1"), 0.0);
	themeStatic.TexX2 = (float) ini.GetDoubleValue(section, _T("TexX1"), 1.0);
	themeStatic.TexY2 = (float) ini.GetDoubleValue(section, _T("TexX2"), 1.0);

	LoadColor(themeStatic.ColRGB, themeStatic.Color);
}

void Themes::LoadThemeStatics(AThemeStatic& themeStaticCollection, const tstring& name)
{
	int sectionNo = 1;
	tstring tempName;
	themeStaticCollection.clear();
	while (ini.GetSectionSize((tempName 
		= (name + boost::lexical_cast<tstring>(sectionNo))).c_str()) > 0)
	{
		themeStaticCollection.push_back(ThemeStatic());
		LoadThemeStatic(themeStaticCollection[sectionNo - 1], tempName);
		++sectionNo;
	}
}

void Themes::LoadThemeButton(ThemeButton& themeButton, const tstring& name, AThemeButtonCollection* themeButtonCollection /*= NULL*/)
{
	if (ini.GetSectionSize(name.c_str()) == 0)
	{
		themeButton.Visible = false;
		return;
	}

	const TCHAR * section = name.c_str();

	themeButton.Visible = ini.GetBoolValue(section, _T("Visible"), true);

	themeButton.Tex = ini.GetValue(section, _T("Tex"), _T(""));
	themeButton.X = ini.GetLongValue(section, _T("X"));
	themeButton.Y = ini.GetLongValue(section, _T("Y"));
	themeButton.Z = (float) ini.GetDoubleValue(section, _T("Z"));
	themeButton.W = ini.GetLongValue(section, _T("W"));
	themeButton.H = ini.GetLongValue(section, _T("H"));
	themeButton.Type = LOOKUP_ENUM_VALUE(TextureType, section, _T("Type"), TextureType::Plain);

	// Reflection
	themeButton.Reflection = ini.GetBoolValue(section, _T("Reflection"), false);
	themeButton.ReflectionSpacing
		= (float) ini.GetDoubleValue(section, _T("ReflectionSpacing"), 15.0);

	themeButton.ColRGB.R  = (float) ini.GetDoubleValue(section, _T("ColR"),  1.0);
	themeButton.ColRGB.G  = (float) ini.GetDoubleValue(section, _T("ColG"),  1.0);
	themeButton.ColRGB.B  = (float) ini.GetDoubleValue(section, _T("ColB"),  1.0);
	themeButton.Int       = (float) ini.GetDoubleValue(section, _T("Int"),   1.0);
	themeButton.DColRGB.R = (float) ini.GetDoubleValue(section, _T("DColR"), 1.0);
	themeButton.DColRGB.G = (float) ini.GetDoubleValue(section, _T("DColG"), 1.0);
	themeButton.DColRGB.B = (float) ini.GetDoubleValue(section, _T("DColB"), 1.0);
	themeButton.DInt      = (float) ini.GetDoubleValue(section, _T("DInt"), 1.0);

	themeButton.Color = ini.GetValue(section, _T("Color"), _T(""));
	themeButton.DColor = ini.GetValue(section, _T("DColor"), _T(""));

	LoadColor(themeButton.ColRGB, themeButton.Color);
	LoadColor(themeButton.DColRGB, themeButton.DColor);

	// Fade
	themeButton.SelectH    = ini.GetLongValue(section, _T("SelectH"), themeButton.H);
	themeButton.SelectW    = ini.GetLongValue(section, _T("SelectW"), themeButton.W);
	themeButton.DeselectReflectionSpacing
		= (float) ini.GetDoubleValue(section, _T("DeSelectReflectionSpacing"), themeButton.ReflectionSpacing);
	themeButton.Fade       = ini.GetBoolValue(section, _T("Fade"), false);
	themeButton.FadeText   = ini.GetBoolValue(section, _T("FadeText"), false);
	themeButton.FadeTex    = ini.GetValue(section, _T("FadeTex"), _T(""));
	themeButton.FadeTexPos = ini.GetLongValue(section, _T("FadeTexPos"), 0);

	if (themeButton.FadeTexPos > 4)      themeButton.FadeTexPos = 4;
	else if (themeButton.FadeTexPos < 0) themeButton.FadeTexPos = 0;

	// Button collection
	size_t parent = ini.GetLongValue(section, _T("Parent"), 0);

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
	int len = ini.GetLongValue(section, _T("Texts"), 0);
	themeButton.Text.assign(len, ThemeText());
	for (int i = 1; i <= len; i++)
		LoadThemeText(themeButton.Text[i - 1], name + _T("Text") + boost::lexical_cast<tstring>(i));
}

void Themes::LoadThemeButtonCollection(ThemeButtonCollection& themeButtonCollection, const tstring& name)
{
	// Default child count to 0
	themeButtonCollection.ChildCount = 0;

	// Load collection style
	LoadThemeButton(themeButtonCollection.Style, name);

	// Load other attributes
	int firstChild = ini.GetLongValue(name.c_str(), _T("FirstChild"), 0);
	if (firstChild <= 0 || firstChild >= MAX_CHILDREN)
		themeButtonCollection.FirstChild = 0;
	else
		themeButtonCollection.FirstChild = firstChild;
}

void Themes::LoadThemeButtonCollections(AThemeButtonCollection& themeButtonCollection, const tstring& name)
{
	int sectionNo = 1;
	tstring tempName;
	themeButtonCollection.clear();
	while (ini.GetSectionSize((tempName 
		= (name + boost::lexical_cast<tstring>(sectionNo))).c_str()) > 0)
	{
		themeButtonCollection.push_back(ThemeButtonCollection());
		LoadThemeButtonCollection(themeButtonCollection[sectionNo - 1], tempName);
		++sectionNo;
	}
}

void Themes::LoadThemeSelectSlide(ThemeSelectSlide& themeSelectSlide, const tstring& name)
{
	const TCHAR * section = name.c_str();

	themeSelectSlide.Text = sLanguage.Translate(ini.GetValue(section, _T("Text"), _T("")));

	themeSelectSlide.Tex = ini.GetValue(section, _T("Tex"), _T(""));
	themeSelectSlide.Type = LOOKUP_ENUM_VALUE(TextureType, section, _T("Type"), TextureType::Plain);

	themeSelectSlide.TexSBG = ini.GetValue(section, _T("TexSBG"), _T(""));
	themeSelectSlide.TypeSBG = LOOKUP_ENUM_VALUE(TextureType, section, _T("TypeSBG"), TextureType::Plain);

	themeSelectSlide.X = ini.GetLongValue(section, _T("X"), 0);
	themeSelectSlide.Y = ini.GetLongValue(section, _T("Y"), 0);
	themeSelectSlide.Z = (float) ini.GetDoubleValue(section, _T("Z"), 0.0);
	themeSelectSlide.W = ini.GetLongValue(section, _T("W"), 0);
	themeSelectSlide.H = ini.GetLongValue(section, _T("H"), 0);

	themeSelectSlide.TextSize = ini.GetLongValue(section, _T("TextSize"), 30);
	themeSelectSlide.SkipX = ini.GetLongValue(section, _T("SkipX"), 0);
	themeSelectSlide.SBGW = ini.GetLongValue(section, _T("SBGW"), 400);

	LoadColor(themeSelectSlide.ColRGB, ini.GetValue(section, _T("Color"), _T("")));
	themeSelectSlide.Int = (float) ini.GetDoubleValue(section, _T("Int"), 1.0);

	LoadColor(themeSelectSlide.DColRGB, ini.GetValue(section, _T("DColor"), _T("")));
	themeSelectSlide.DInt = (float) ini.GetDoubleValue(section, _T("DInt"), 1.0);

	LoadColor(themeSelectSlide.TColRGB, ini.GetValue(section, _T("TColor"), _T("")));
	themeSelectSlide.TInt = (float) ini.GetDoubleValue(section, _T("TInt"), 1.0);

	LoadColor(themeSelectSlide.TDColRGB, ini.GetValue(section, _T("TDColor"), _T("")));
	themeSelectSlide.TDInt = (float) ini.GetDoubleValue(section, _T("TDInt"), 1.0);

	LoadColor(themeSelectSlide.SBGColRGB, ini.GetValue(section, _T("SBGColor"), _T("")));
	themeSelectSlide.SBGInt = (float) ini.GetDoubleValue(section, _T("SBGInt"), 1.0);

	LoadColor(themeSelectSlide.SBGDColRGB, ini.GetValue(section, _T("SBGDColor"), _T("")));
	themeSelectSlide.SBGDInt = (float) ini.GetDoubleValue(section, _T("SBGDInt"), 1.0);

	LoadColor(themeSelectSlide.STColRGB, ini.GetValue(section, _T("STColor"), _T("")));
	themeSelectSlide.STInt = (float) ini.GetDoubleValue(section, _T("STInt"), 1.0);

	LoadColor(themeSelectSlide.STDColRGB, ini.GetValue(section, _T("STDColor"), _T("")));
	themeSelectSlide.STDInt = (float) ini.GetDoubleValue(section, _T("STDInt"), 1.0);

	themeSelectSlide.ShowArrows = ini.GetBoolValue(section, _T("ShowArrays"), false);
	themeSelectSlide.OneItemOnly = ini.GetBoolValue(section, _T("OneItemOnly"), false);
}

void Themes::LoadThemeEqualizer(ThemeEqualizer& themeEqualizer, const tstring& name)
{
	const TCHAR * section = name.c_str();

	themeEqualizer.Visible = ini.GetBoolValue(section, _T("Visible"), false);
	themeEqualizer.Direction = ini.GetBoolValue(section, _T("Direction"), false);
	themeEqualizer.Alpha = (float) ini.GetDoubleValue(section, _T("Alpha"), 1.0);
	themeEqualizer.Space = ini.GetLongValue(section, _T("Space"), 1);
	themeEqualizer.X = ini.GetLongValue(section, _T("X"), 0);
	themeEqualizer.Y = ini.GetLongValue(section, _T("Y"), 0);
	themeEqualizer.Z = (float) ini.GetDoubleValue(section, _T("Z"), 1.0);
	themeEqualizer.W = ini.GetLongValue(section, _T("PieceW"), 8);
	themeEqualizer.H = ini.GetLongValue(section, _T("PieceH"), 8);
	themeEqualizer.Bands = ini.GetLongValue(section, _T("Bands"), 5);
	themeEqualizer.Length = ini.GetLongValue(section, _T("Length"), 12);
	themeEqualizer.Reflection = ini.GetBoolValue(section, _T("Reflection"), false);
	themeEqualizer.ReflectionSpacing
		= (float) ini.GetDoubleValue(section, _T("ReflectionSpacing"), 15.0);

	LoadColor(themeEqualizer.ColRGB, ini.GetValue(section, _T("Color"), _T("Black")));
}

/**
 * Looks up a given theme by its name.
 *
 * @param	themeName	Name of the theme to lookup.
 *
 * @return	NULL if it fails, else a pointer to the associated ThemeEntry.
 */
ThemeEntry * Themes::LookupTheme(tstring themeName)
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
ThemeEntry * Themes::LookupThemeDefault(tstring themeName, tstring defaultTheme)
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
