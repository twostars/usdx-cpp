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

#ifndef _GRAPHIC_H
#define _GRAPHIC_H
#pragma once

#include "Texture.h"

void Initialize3D(const char * windowTitle);
void LoadFontTextures();
void UnloadFontTextures();
void LoadLoadingScreen();
void LoadTextures();
void LoadScreens();
void SwapBuffers();

SDL_Surface * LoadSurfaceFromFile(const path& filename);
void UnloadSurface(SDL_Surface * texSurface);

void AdjustPixelFormat(SDL_Surface *& texSurface, eTextureType textureType);
bool PixelFormatEquals(SDL_PixelFormat * fmt1, const SDL_PixelFormat * fmt2);
void ScaleImage(SDL_Surface * imgSurface, uint32 width, uint32 height);
void FitImage(SDL_Surface *& imgSurface, uint32 width, uint32 height);
void ColorizeImage(SDL_Surface * imgSurface, uint32 newColor);

void glColorRGB(const RGB& color);
void glColorRGB(const RGB& color, float alpha);
void glColorRGB(const RGBA& color);
void glColorRGB(const RGBA& color, float alpha);
void glColorRGBInt(const RGB& color, float intensity);
void glColorRGBInt(const RGB& color, float alpha, float intensity);
void glColorRGBInt(const RGBA& color, float intensity);
void glColorRGBInt(const RGBA& color, float alpha, float intensity);

void FreeGfxResources();

/* TODO: Clean up these globals */

typedef std::set<SDL_Surface *> SurfaceCollection;

extern SDL_Window  * Screen;
extern SDL_GLContext GLContext;

extern SurfaceCollection g_surfaces;

extern int RenderW, RenderH;
extern int ScreenW, ScreenH;
extern int Screens, ScreenAct, ScreenX;

extern bool Fullscreen;

// Screens
extern class ScreenLoading			* UILoading;
extern class ScreenMain				* UIMain;
extern class ScreenName				* UIName;
extern class ScreenLevel			* UILevel;
extern class ScreenSong				* UISong;
extern class ScreenSing				* UISing;
extern class ScreenScore			* UIScore;
extern class ScreenTop5				* UITop5;
extern class ScreenOptions			* UIOptions;
extern class ScreenOptionsGame		* UIOptionsGame;
extern class ScreenOptionsGraphics	* UIOptionsGraphics;
extern class ScreenOptionsSound		* UIOptionsSound;
extern class ScreenOptionsLyrics	* UIOptionsLyrics;
extern class ScreenOptionsThemes	* UIOptionsThemes;
extern class ScreenOptionsRecord	* UIOptionsRecord;
extern class ScreenOptionsAdvanced	* UIOptionsAdvanced;
extern class ScreenEditSub			* UIEditSub;
extern class ScreenEdit				* UIEdit;
extern class ScreenEditConvert		* UIEditConvert;
extern class ScreenEditHeader		* UIEditHeader;
extern class ScreenOpen				* UIOpen;

extern class ScreenSongMenu			* UISongMenu;
extern class ScreenSongJumpTo		* UISongJumpTo;

// Party screens
extern class ScreenPartyNewRound	* UIPartyNewRound;
extern class ScreenPartyScore		* UIPartyScore;
extern class ScreenPartyWin			* UIPartyWin;
extern class ScreenPartyOptions		* UIPartyOptions;
extern class ScreenPartyPlayer		* UIPartyPlayer;
extern class ScreenPartyRounds		* UIPartyRounds;

// Stats screens
extern class ScreenStatMain			* UIStatMain;
extern class ScreenStatDetail		* UIStatDetail;

// Credits Screen
extern class ScreenCredits			* UICredits;

// Popups
extern class ScreenPopupCheck		* UIPopupCheck;
extern class ScreenPopupError		* UIPopupError;
extern class ScreenPopupInfo		* UIPopupInfo;

// Notes
extern Texture TexNoteLeft[6];      // formerly Tex_Left
extern Texture TexNoteMid[6];       // formerly Tex_Mid
extern Texture TexNoteRight[6];     // formerly Tex_Right

extern Texture TexNoteBGLeft[6];    // formerly Tex_plain_Left
extern Texture TexNoteBGMid[6];     // formerly Tex_plain_Mid
extern Texture TexNoteBGRight[6];   // formerly Tex_plain_Right

extern Texture TexNoteGlowLeft[6];  // formerly Tex_BG_Left
extern Texture TexNoteGlowMid[6];   // formerly Tex_BG_Mid
extern Texture TexNoteGlowRight[6]; // formerly Tex_BG_Right

extern Texture TexNoteStar, TexNotePerfectStar;

extern Texture TexBall, TexLyricHelpBar;
extern Texture TexTimeProgress;

// Sing Bar Mod
extern Texture TexSingBarBack, TexSingBar, TexSingBarFront;

// PhrasenBonus - Line Bonus Mod
extern Texture TexSingLineBonusBack[9];

// ScoreBG Textures
extern Texture TexScoreBG[6];

// Score Screen Textures
extern Texture TexScoreNoteBarLevelDark[6];
extern Texture TexScoreNoteBarRoundDark[6];

extern Texture TexScoreNoteBarLevelLight[6];
extern Texture TexScoreNoteBarRoundLight[6];

extern Texture TexScoreNoteBarLevelLightest[6];
extern Texture TexScoreNoteBarRoundLightest[6];

extern Texture TexScoreRatings[8];

// arrows for SelectSlide
extern Texture TexSelectSArrowL, TexSelectSArrowR;

// textures for software mouse cursor
extern Texture TexCursorUnpressed, TexCursorPressed;

extern bool PboSupported;

#endif
