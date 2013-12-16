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
#include "TextGL.h"

// Build all fonts
void BuildFonts()
{
}

// Deletes all fonts
void KillFonts()
{
}

// Returns text width
float glTextWidth(const tstring& text)
{
	/*
	BoundsDbl bounds = Fonts[ActiveFont].Font.BBox(text, true);
	return bounds.Right - bounds.Left;
	*/
	return 0.0f;
}

// Custom OpenGL print routine
void glPrint(const tstring& text, ...)
{
	if (text.empty())
		return;

	/*
	GLFont& font = Fonts[ActiveFont];
	glPushMatrix();
		// Set font position
		glTranslatef(font.X, font.Y + font.Ascender, font.Z);

		// Draw string
		// font.Font.Print(text);
	glPopMatrix();
	*/
}

// Reset settings for active font
void ResetFont()
{
	SetFontPos(0.0f, 0.0f);
	SetFontZ(0.0f);
	SetFontItalic(false);
	SetFontReflection(false, 0.0f);
}

// Sets X, Y position of font
void SetFontPos(float X, float Y)
{
	/*
	Fonts[ActiveFont].X = X;
	Fonts[ActiveFont].Y = Y;
	*/
}

// Sets Z position of float
void SetFontZ(float Z)
{
	// Fonts[ActiveFont].Z = Z;
}

// Sets font size
void SetFontSize(float height)
{
	// Fonts[ActiveFont].Font.Height = height;
}

// Sets active font style (normal, bold, etc)
void SetFontStyle(uint32 style)
{
	// ActiveFont = style;
}

// Enables/disables italics.
void SetFontItalic(bool enable)
{
	// GLFont& font = Fonts[ActiveFont];
	// if (enable)
	//	font.Font.Style += *Italic;
	// else
	//	font.Font.Stle -= *Italic;
}

// Enables/disables text reflection.
void SetFontReflection(bool enable, float spacing)
{
	/*
	GLFont& font = Fonts[ActiveFont];

	if (enable)
		font.Font.Style += *Reflect;
	else
		font.Font.Style -= *Reflect;

	font.Font.ReflectionSpacing = spacing + font.Font.Descender;
	*/
}
