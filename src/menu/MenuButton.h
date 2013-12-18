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

#ifndef _MENUBUTTON_H
#define _MENUBUTTON_H
#pragma once

class Texture;
class MenuButton
{
public:
	MenuButton();
	MenuButton(Texture* tex);
	MenuButton(Texture* tex, Texture* deselectTexture);

private:
	void Create();

public:
	void SetX(float value);
	void SetY(float value);
	void SetZ(float value);
	void SetW(float value);
	void SetH(float value);

	virtual void SetSelect(bool value);
	virtual void Draw();

	MouseOverRect GetMouseOverRect();

	std::vector<MenuText> Texts;
	Texture* Tex;
	Texture* Tex2;

	bool Colorized;
	Texture* DeselectTexture;

	Texture* FadeTex;
	uint8 FadeTexPos;

	int DeselectType; // not used yet
	bool Visible;

	bool Reflection;
	float ReflectionSpacing;
	float DeselectReflectionSpacing;

	bool Fade;
	bool FadeText;
	
	bool Selectable;

	uint8 Parent;

	RGB SelectColRGB;
	float SelectInt;
	float SelectTInt;

	RGB DeselectColRGB;
	float DeselectInt;
	float DeselectTInt;

	float SelectW;
	float SelectH;

	bool Selected;

	float FadeProgress;
	uint32 FadeLastTick;

	float DeselectW, DeselectH;
	float PosX, PosY;
};

#endif
