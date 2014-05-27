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

#ifndef _MENUTEXT_H
#define _MENUTEXT_H
#pragma once

class MenuText
{
private:
	struct ParseData
	{
		tsize_type			NextPos,	// next pos of a space etc
							LastPos,	// last pos "
							LastBreak,	// pos of last break
							FirstWord;	// is first word after break?
		bool				IsBreak;	// true if the break is not caused
										// because the text is out of the area
	};

public:
	MenuText();
	MenuText(float x, float y, const tstring& text);
	MenuText(float x, float y, float w, uint32 style, 
		float size, const RGB& rgb, int align, const tstring& text, 
		bool reflection, float reflectionSpacing, 
		float z);

	void Create(float x, float y, float w, uint32 style,
				float size, const RGB& rgb, int align, const tstring& text,
				bool reflection, float reflectionSpacing,
				float z);

	void EnableBlinkingCursor();
	void SetSelected(bool value);
	void SetText(const tstring& text);

private:
	bool GetNextPos(ParseData& data);
	void AddBreak(ParseData& data, tsize_type from, tsize_type to);

public:
	void DeleteLastLetter();
	void Draw();

	INLINE bool IsSelected() { return Selected; }
	INLINE const tstring& GetText() { return TextString; }

	float X, Y, Z;
	float MoveX, MoveY; // modifier for position that doesn't affect the real coordinate
	float W;
	float Size;
	RGB ColRGB;
	float Alpha;
	float Int;
	uint32 Style;
	bool Visible;
	// TODO: Use an enum.
	int Align;	// 0 = left, 1 = center, 2 = right
	bool Reflection;
	float ReflectionSpacing;

protected:
	bool Selected;

public:
	tstring TextString;
	std::vector<tstring> TextTiles;

	uint32 STicks;
	bool SelectBlink;
};

#endif
