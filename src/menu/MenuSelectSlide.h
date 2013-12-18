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

#ifndef _MENUSELECTSLIDE_H
#define _MENUSELECTSLIDE_H
#pragma once

class Texture;
class MenuSelectSlide
{
public:
	MenuSelectSlide();

	void SetSelect(bool value);
	void SetSelectOpt(int value);
	INLINE int GetSelectOpt() { return SelectOptInt; }

private:
	void DoSelection(size_t sel);
public:
	void Draw();

	// Automatically generate lines (texts)
	void GenerateLines();

	MouseOverRect GetMouseOverRect();
	MouseClickAction OnClick(float x, float y);

public:
	bool Selected;

private:
	tstring AdjustOptionTextToFit(const tstring& optionText);

public:
	MenuText Text; // Main text describing option
	std::vector<MenuText> TextOptions; // Texts in the position of possible options
	std::vector<tstring>  TextOptionNames; // Names of the possible options

	Texture* Tex;		// Select texture
	Texture* TexSBG;	// Selection background texture

	bool Colorized;
	Texture* DeselectTexture;
	bool ColorizedSBG;
	Texture* DeselectTextureSBG;

	Texture* Tex_SelectS_ArrowL;
	Texture* Tex_SelectS_ArrowR;

	int SelectOptInt;
	// int* PData;
	int Data;

	// For automatically setting line count
	uint8 Lines;

	// Whether to show one item or all that fits into the selection.
	bool OneItemOnly; // default is false

	bool Visible;

	// Main static
	RGB ColRGB;
	float Int;
	RGB DColRGB;
	float DInt;

	// Main text
	RGB TColRGB;
	float TInt;
	RGB TDColRGB;
	float TDInt;

	// Selection background static
	RGB SBGColRGB;
	float SBGInt;
	RGB SBGDColRGB;
	float SBGDInt;

	// Selection text
	RGB STColRGB;
	float STInt;
	RGB STDColRGB;
	float STDInt;
};

#endif
