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

#ifndef _THEME_DEFINES_H
#define _THEME_DEFINES_H
#pragma once

#define MAX_CHILDREN 256

#include "ThemeTypes.h"

class ThemeBasic
{
public:
	ThemeBackground Background;
	AThemeText      Text;
	AThemeStatic    Static;
	AThemeButtonCollection ButtonCollection;

	ThemeBasic() {}
	virtual ~ThemeBasic() {}
};

class ThemeLoading : public ThemeBasic
{
public:
	ThemeStatic StaticAnimation;
	ThemeText   TextLoading;
};

class ThemeMain : public ThemeBasic
{
public:
	ThemeButton ButtonSolo;
	ThemeButton ButtonMulti;
	ThemeButton ButtonStats;
	ThemeButton ButtonEditor;
	ThemeButton ButtonOptions;
	ThemeButton ButtonExit;

	ThemeText   TextDescription;
	ThemeText   TextDescriptionLong;

	std::string     Description[MAX_PLAYERS];
	std::string     DescriptionLong[MAX_PLAYERS];
};

class ThemeName : public ThemeBasic
{
public:
	ThemeButton ButtonPlayer[MAX_PLAYERS];
};

class ThemeLevel : public ThemeBasic
{
public:
	ThemeButton ButtonEasy;
	ThemeButton ButtonMedium;
	ThemeButton ButtonHard;
};

class ThemeSong : public ThemeBasic
{
public:
	ThemeText    TextArtist;
	ThemeText    TextTitle;
	ThemeText    TextNumber;

	ThemeStatic  VideoIcon;

	ThemeStatic  MedleyIcon;
	ThemeStatic  CalculatedMedleyIcon;

	// Show category in top left
	ThemeText    TextCat;
	ThemeStatic  StaticCat;

	struct
	{
		bool Reflections;
		int X, Y, Z;
		int W, H;
		int Style;
	} Cover;

	ThemeEqualizer Equalizer;

	// Party & non-party specific statics & text
	AThemeStatic StaticParty;
	AThemeText   TextParty;
	AThemeStatic StaticNonParty;
	AThemeText   TextNonParty;

	// Party mode
	ThemeStatic   StaticTeamJoker[MAX_TEAMS][MAX_JOKERS];
};

class ThemeSing : public ThemeBasic
{
public:
	ThemeStatic StaticTimeProgress;
	ThemeText   TextTimeText;

	ThemeStatic StaticP1;
	ThemeText   TextP1;
	ThemeStatic StaticP1ScoreBG;
	ThemeText   TextP1Score;

	// moveable singbar
	ThemeStatic StaticP1SingBar;
	ThemeStatic StaticP1ThreePSingBar;
	ThemeStatic StaticP1TwoPSingBar;
	ThemeStatic StaticP2RSingBar;
	ThemeStatic StaticP2MSingBar;
	ThemeStatic StaticP3SingBar;
	//
	
	// added for PS3 skin
	// game in 2/4 player
	ThemeStatic StaticP1TwoP;
	ThemeStatic StaticP1TwoPScoreBG;
	ThemeText   TextP1TwoP;
	ThemeText   TextP1TwoPScore;

	// game in 3/6 player
	ThemeStatic StaticP1ThreeP;
	ThemeStatic StaticP1ThreePScoreBG;
	ThemeText   TextP1ThreeP;
	ThemeText   TextP1ThreePScore;
	//
	
	ThemeStatic StaticP2R;
	ThemeStatic StaticP2RScoreBG;
	ThemeText   TextP2R;
	ThemeText   TextP2RScore;

	ThemeStatic StaticP2M;
	ThemeStatic StaticP2MScoreBG;
	ThemeText   TextP2M;
	ThemeText   TextP2MScore;

	ThemeStatic StaticP3R;
	ThemeStatic StaticP3RScoreBG;
	ThemeText   TextP3R;
	ThemeText   TextP3RScore;

	ThemeStatic StaticSongName;
	ThemeText   TextSongName;

	// Line bonus translations
	std::string     LineBonusText[9];

	ThemeStatic PausePopup;
};

class ThemeLyricBar /* not a proper theme */
{
public:
	int IndicatorYOffset;
	int UpperX, UpperW, UpperY, UpperH;
	int LowerX, LowerW, LowerY, LowerH;
};

class ThemeScore : public ThemeBasic
{
public:
	ThemeText     TextArtist;
	ThemeText     TextTitle;
	ThemeText     TextArtistTitle;

	AThemeStatic  PlayerStatic        [MAX_PLAYERS];
	AThemeText    PlayerTexts         [MAX_PLAYERS];

	ThemeText     TextName            [MAX_PLAYERS];
	ThemeText     TextScore           [MAX_PLAYERS];

	ThemeText     TextNotes           [MAX_PLAYERS];
	ThemeText     TextNotesScore      [MAX_PLAYERS];
	ThemeText     TextLineBonus       [MAX_PLAYERS];
	ThemeText     TextLineBonusScore  [MAX_PLAYERS];
	ThemeText     TextGoldenNotes     [MAX_PLAYERS];
	ThemeText     TextGoldenNotesScore[MAX_PLAYERS];
	ThemeText     TextTotal           [MAX_PLAYERS];
	ThemeText     TextTotalScore      [MAX_PLAYERS];

	ThemeStatic   StaticBoxLightest   [MAX_PLAYERS];
	ThemeStatic   StaticBoxLight      [MAX_PLAYERS];
	ThemeStatic   StaticBoxDark       [MAX_PLAYERS];

	ThemeStatic   StaticRatings       [MAX_PLAYERS];

	ThemeStatic   StaticBackLevel     [MAX_PLAYERS];
	ThemeStatic   StaticBackLevelRound[MAX_PLAYERS];
	ThemeStatic   StaticLevel         [MAX_PLAYERS];
	ThemeStatic   StaticLevelRound    [MAX_PLAYERS];
	ThemeStatic   StaticPlayerIdBox   [MAX_PLAYERS];

	ThemeStatic   StaticNavigate;
	ThemeText     TextNavigate;
};

class ThemeTop5 : public ThemeBasic
{
public:
	ThemeText     TextLevel;
	ThemeText     TextArtistTitle;

	AThemeStatic  StaticNumber;
	AThemeText    TextNumber;
	AThemeText    TextName;
	AThemeText    TextScore;
	AThemeText    TextDate;
};

class ThemeOptions : public ThemeBasic
{
public:
	ThemeButton   ButtonGame;
	ThemeButton   ButtonGraphics;
	ThemeButton   ButtonSound;
	ThemeButton   ButtonLyrics;
	ThemeButton   ButtonThemes;
	ThemeButton   ButtonRecord;
	ThemeButton   ButtonAdvanced;
	ThemeButton   ButtonExit;

	ThemeText     TextDescription;
	std::string       Description[8]; /* 8 buttons */
};

class ThemeOptionsGame : public ThemeBasic
{
public:
	ThemeSelectSlide SelectPlayers;
	ThemeSelectSlide SelectDifficulty;
	ThemeSelectSlide SelectLanguage;
	ThemeSelectSlide SelectTabs;
	ThemeSelectSlide SelectSorting;
	ThemeSelectSlide SelectDebug;
	ThemeButton      ButtonExit;
};

class ThemeOptionsGraphics : public ThemeBasic
{
public:
	ThemeSelectSlide SelectFullscreen;
	ThemeSelectSlide SelectResolution;
	ThemeSelectSlide SelectDepth;
	ThemeSelectSlide SelectVisualizer;
	ThemeSelectSlide SelectOscilloscope;
	ThemeSelectSlide SelectLineBonus;
	ThemeSelectSlide SelectMovieSize;
	ThemeButton      ButtonExit;
};

class ThemeOptionsSound : public ThemeBasic
{
public:
	ThemeSelectSlide SelectMicBoost;
	ThemeSelectSlide SelectBackgroundMusic;
	ThemeSelectSlide SelectClickAssist;
	ThemeSelectSlide SelectBeatClick;
	ThemeSelectSlide SelectThreshold;
	ThemeSelectSlide SelectSlidePreviewVolume;
	ThemeSelectSlide SelectSlidePreviewFading;
	ThemeSelectSlide SelectSlideVoicePassthrough;
	ThemeButton      ButtonExit;
};

class ThemeOptionsLyrics : public ThemeBasic
{
public:
	ThemeSelectSlide SelectLyricsFont;
	ThemeSelectSlide SelectLyricsEffect;
	ThemeSelectSlide SelectNoteLines;
	ThemeButton      ButtonExit;
};

class ThemeOptionsThemes : public ThemeBasic
{
public:
	ThemeSelectSlide SelectTheme;
	ThemeSelectSlide SelectSkin;
	ThemeSelectSlide SelectColor;
	ThemeButton      ButtonExit;
};

class ThemeOptionsRecord : public ThemeBasic
{
public:
	ThemeSelectSlide SelectSlideCard;
	ThemeSelectSlide SelectSlideInput;
	ThemeSelectSlide SelectSlideChannel;
	ThemeButton      ButtonExit;
};

class ThemeOptionsAdvanced : public ThemeBasic
{
public:
	ThemeSelectSlide SelectLoadAnimation;
	ThemeSelectSlide SelectEffectSing;
	ThemeSelectSlide SelectScreenFade;
	ThemeSelectSlide SelectLineBonus;
	ThemeSelectSlide SelectAskBeforeDel;
	ThemeSelectSlide SelectOnSongClick;
	ThemeSelectSlide SelectPartyPopup;
	ThemeButton      ButtonExit;
};

class ThemeEdit : public ThemeBasic
{
public:
	ThemeButton      ButtonConvert;
	ThemeButton      ButtonExit;

	ThemeText        TextDescription;
	ThemeText        TextDescriptionLong;
	std::string          Description[MAX_PLAYERS];
	std::string          DescriptionLong[MAX_PLAYERS];
};

class ThemeErrorPopup : public ThemeBasic
{
public:
	ThemeButton      Button1;
	ThemeText        TextError;
};

class ThemeCheckPopup : public ThemeBasic
{
public:
	ThemeButton      Button1;
	ThemeButton      Button2;
	ThemeText        TextCheck;
};

class ThemeSongMenu : public ThemeBasic
{
public:
	ThemeButton      Button1;
	ThemeButton      Button2;
	ThemeButton      Button3;
	ThemeButton      Button4;

	ThemeSelectSlide SelectSlide3;

	ThemeText        TextMenu;
};

class ThemeSongJumpTo : public ThemeBasic
{
public:
	ThemeButton      ButtonSearchText;
	ThemeSelectSlide SelectSlideType;
	ThemeText        TextFound;

	// Translated texts
	std::string          SongsFound;
	std::string          NoSongsFound;
	std::string          CatText;
	std::string          IType[3];
};

class ThemePartyNewRound : public ThemeBasic
{
public:
	ThemeText        TextRound[MAX_ROUNDS];
	ThemeText        TextWinner[MAX_ROUNDS];
	ThemeText        TextNextRound;
	ThemeText        TextNextRoundNo;
	ThemeStatic      StaticRound[MAX_ROUNDS];

	struct Team
	{
		ThemeText    TextNextPlayer;
		ThemeText    TextScore;
		ThemeText    TextName;
		ThemeText    TextPlayers;
		ThemeStatic  StaticTeam;
		ThemeStatic  StaticNextPlayer;
	};

	Team             Teams[MAX_TEAMS];
};

class ThemePartyScore : public ThemeBasic
{
public:
	struct Team
	{
		ThemeText     TextScore;
		ThemeText     TextName;
		ThemeStatic   StaticTeam;
		ThemeStatic   StaticTeamBG;
		ThemeStatic   StaticTeamDeco;
	};

	Team              Teams[MAX_TEAMS];
	ThemeText         TextWinner;

	struct
	{
		bool           ChangeTextures;

		std::string        Texture [MAX_TEAMS];
		eTextureType   Type    [MAX_TEAMS];
		std::string        Color   [MAX_TEAMS];
	} DecoTextures;
};

class ThemePartyWin : public ThemeBasic
{
public:
	struct Team
	{
		ThemeText      TextScore;
		ThemeText      TextName;
		ThemeStatic    StaticTeam;
		ThemeStatic    StaticTeamBG;
		ThemeStatic    StaticTeamDeco;
	};

	Team               Teams[MAX_TEAMS];
	ThemeText          TextWinner;
};

class ThemePartyOptions : public ThemeBasic
{
public:
	ThemeSelectSlide   SelectLevel;
	ThemeSelectSlide   SelectPlaylist;
	ThemeSelectSlide   SelectPlaylist2;
};

class ThemePartyPlayer : public ThemeBasic
{
public:
	ThemeSelectSlide     SelectTeams;

	struct Team
	{
		ThemeButton      TeamName;
		ThemeSelectSlide SelectPlayers;
		ThemeButton      PlayerName[MAX_PLAYERS_PER_TEAM];
	};

	Team                 Teams[MAX_TEAMS];
};

class ThemePartyRounds : public ThemeBasic
{
public:
	ThemeSelectSlide     SelectRoundCount;
	ThemeSelectSlide     SelectRound[MAX_ROUNDS];
};

class ThemeStatMain : public ThemeBasic
{
public:
	ThemeButton          ButtonScores;
	ThemeButton          ButtonSingers;
	ThemeButton          ButtonSongs;
	ThemeButton          ButtonBands;
	ThemeButton          ButtonExit;

	ThemeText            TextOverview;
};

class ThemeStatDetail : public ThemeBasic
{
public:
	ThemeButton          ButtonNext;
	ThemeButton          ButtonPrev;
	ThemeButton          ButtonReverse;
	ThemeButton          ButtonExit;

	ThemeText            TextDescription;
	ThemeText            TextPage;
	AThemeText           TextList;

	std::string              Description [MAX_PLAYERS_PER_TEAM];
	std::string              DescriptionR[MAX_PLAYERS_PER_TEAM];
	std::string              FormatStr   [MAX_PLAYERS_PER_TEAM];
	std::string              PageStr;
};

class ThemePlaylist /* not a proper theme */
{
public:
	std::string CatText;
};


#endif
