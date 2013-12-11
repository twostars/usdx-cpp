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

#ifndef _THEME_TYPES_H
#define _THEME_TYPES_H
#pragma once

#include "Texture.h"

enum BackgroundType
{
	bgtNone, bgtColor, bgtTexture, bgtVideo, bgtFade, bgtAuto
};

const tstring BGT_Names[] = 
{
	_T("none"), _T("color"), _T("texture"), _T("video"), _T("fade"), _T("audio") 
};

struct ThemeBackground
{
	BackgroundType BGType;
	RGB Color;
	tstring Tex;
	float Alpha;
};

// Default background for screens without a theme
const ThemeBackground DEFAULT_BACKGROUND =
{
	/* BGType: */ bgtColor, 
	/* Color : */ { 1.0f, 1.0f, 1.0f }, 
	/* Tex   : */ _T(""), 
	/* Alpha : */ 1.0f
};

struct ThemeStatic
{
	int X, Y;
	float Z;
	int W, H;
	std::string Color;
	RGB ColRGB;
	tstring Tex;
	eTextureType Type;
	float TexX1, TexY1;
	float TexX2, TextY2;

	// Reflection
	bool Reflection;
	float ReflectionSpacing;
};

typedef std::vector<ThemeStatic> AThemeStatic;

struct ThemeText
{
	int X, Y, W;
	float Z;
	std::string Color;
	RGB ColRGB;
	int Font;
	int Size;
	int Align;
	tstring Text;

	// Reflection
	bool Reflection;
	float ReflectionSpacing;
};

typedef std::vector<ThemeText> AThemeText;

struct ThemeButton
{
	AThemeText Text;
	int X, Y;
	float Z;
	int W, H;
	std::string Color;
	RGB ColRGB;
	float Int; // intensity
	std::string DColor;
	RGB DColRGB;
	float DInt; // intensity
	tstring Tex;
	eTextureType Type;

	bool Visible;

	// Reflection
	bool Reflection;
	float ReflectionSpacing;

	// Fade
	int SelectH, SelectW;
	bool Fade;
	bool FadeText;
	float DeselectReflectionSpacing;
	tstring FadeTex;
	int FadeTexPos;

	// Button collection
	uint8 Parent; // Number (ID) of the button collection this button is assigned to. 0 for no assignment.
};

struct ThemeButtonCollection
{
	ThemeButton Style;
	uint8 ChildCount; // number of assigned children
	uint8 FirstChild; // number of child whose interaction position the button should be
};

typedef std::vector<ThemeButtonCollection> AThemeButtonCollection;

struct ThemeSelectSlide
{
	tstring Tex;
	eTextureType Type;
	tstring TexSBG;
	eTextureType TypeSBG;
	int X, Y, W, H;
	float Z;
	int SBGW;
	int TextSize;

	bool ShowArrows;
	bool OneTimeOnly;

	tstring Text;

	RGB ColRGB;
	float Int; // intensity

	RGB DColRGB;
	float DInt; // intensity

	RGB TColRGB;
	float TInt; // intensity

	RGB TDColRGB;
	float TDInt; // intensity

	RGB SBGColRGB;
	float SBGInt; // intensity

	RGB SBGDColRGB;
	float SBGDInt; // intensity

	RGB STColRGB;
	float STInt; // intensity

	RGB STDColRGB;
	float STDInt; // intensity

	int SkipX;
};

struct ThemeEqualizer
{
	bool Visible;
	bool Direction;
	float Alpha;
	int X, Y;
	float Z;
	int W, H;
	int Space;
	int Bands;
	int Length;
	RGB ColRGB;

	// Reflection
	bool Reflection;
	float ReflectionSpacing;
};

#endif
