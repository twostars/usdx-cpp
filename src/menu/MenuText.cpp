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
#include "MenuText.h"
#include "../base/Graphic.h"
#include "../base/TextGL.h"

MenuText::MenuText()
{
	RGB rgbDefault = { 0.0f, 0.0f, 0.0f };
	std::string textDefault;
	float sizeDefault = 30.0f;

	Create(0, 0, 0, ftNormal, sizeDefault, rgbDefault, 0, textDefault, false, 0.0f, 0.0f);
}

MenuText::MenuText(float x, float y, const std::string& text)
{
	RGB rgbDefault = { 0.0f, 0.0f, 0.0f };
	float sizeDefault = 30.0f;

	Create(x, y, 0, ftNormal, sizeDefault, rgbDefault, 0, text, false, 0.0f, 0.0f);
}

MenuText::MenuText(float x, float y, float w, uint32 style, 
				   float size, const RGB& rgb, int align, const std::string& text, 
				   bool reflection, float reflectionSpacing, 
				   float z)
{
	Create(x, y, w, style, size, rgb, align, text, reflection, reflectionSpacing, z);
}

void MenuText::Create(float x, float y, float w, uint32 style,
					  float size, const RGB& rgb, int align, const std::string& text,
					  bool reflection, float reflectionSpacing,
					  float z)
{
	Alpha = 1.0f;
	Int = 1.0f;
	X = x, Y = y, Z = z, W = w;
	MoveX = MoveY = 0.0f;
	Style = style;
	Size = size;
	ColRGB = rgb;
	Align = align;
	TextString = text;
	Reflection = reflection;
	ReflectionSpacing = reflectionSpacing;
	Selected = false;
	SelectBlink = false;
	Visible = true;
	STicks = 0;
}

void MenuText::EnableBlinkingCursor()
{
	// set cursor visible
	SelectBlink = true;
	STicks = (uint32)((float) SDL_GetTicks() / 550.0f);
}

void MenuText::SetSelected(bool value)
{
	Selected = value;
	EnableBlinkingCursor();
}

/**
 * Sets the menu text label & breaks it up into blocks if applicable.
 *
 * @exception	NotImplementedException	Thrown when an unhandled linebreak condition occurs.
 *
 * @param	text	The text.
 */

void MenuText::SetText(const std::string& text)
{
	TextString = text;
	EnableBlinkingCursor();

	// Reset tiles
	TextTiles.clear();

	// Break out now if there is no need to create tiles
	if (W <= 0 
		&& strchr(text.c_str(), '\n') == NULL)
	{
		TextTiles.push_back(text);
		return;
	}

	ParseData data = {0};

	// Set font properties.
	if (W > 0)
	{
		SetFontStyle(Style);
		SetFontSize(Size);
	}

	// Go through text
	while (GetNextPos(data))
	{
		// Break in text found
		if (data.IsBreak)
		{
			// Look for break before the break
			if ((data.NextPos - data.LastPos) > 1 
				&& glTextWidth(TextString.substr(data.LastBreak, data.NextPos - data.LastBreak + 1)) > W)
			{
				data.IsBreak = false;

				// Not the first word after break, so we don't have to break within a word
				if (data.FirstWord > 1)
				{
					// add break before actual position, because there the text fits the area
					AddBreak(data, data.LastBreak, data.LastPos);
				}
				// First word after break, break within the word.
				else
				{
					// TODO
					throw NotImplementedException("MenuText::SetMenuText(): First word after break, break within the word. [1]");
				}
			}

			data.IsBreak = true;

			// Add break from text
			AddBreak(data, data.LastBreak, data.NextPos);
		}
		// Text comes out of the text area -> create break
		else if (glTextWidth(TextString.substr(data.LastBreak, data.NextPos - data.LastBreak + 1)) > W)
		{
			// Not the first word after break, so we don't have to break within a word
			if (data.FirstWord > 1)
			{
				// Add break before actual position, because there the text fits the area
				AddBreak(data, data.LastBreak, data.LastPos);
			}
			// First word after break, break within the word.
			else
			{
				// TODO
				throw NotImplementedException("MenuText::SetMenuText(): First word after break, break within the word. [2]");
			}
		}

		++data.FirstWord;
	}

	// Add ending
	AddBreak(data, data.LastBreak, TextString.length() + 1);
}

bool MenuText::GetNextPos(ParseData& data)
{
	int T1, T2;

	data.LastPos = data.NextPos;

	// Next space (if width is given)
	if (W > 0)
		T1 = strpos(TextString.c_str() + data.LastPos + 1, " ");
	else
		T1 = TextString.length();

	// Next break
	T2 = strpos(TextString.c_str() + data.LastPos + 1, "\n");

	if (T1 < 0)
		T1 = TextString.length();

	if (T2 < 0)
		T2 = TextString.length();

	// Get nearest pos
	data.NextPos = std::min(T1, T2);

	if (data.LastPos == TextString.length())
		data.NextPos = 0;

	data.IsBreak = (data.NextPos == T2 && data.NextPos != TextString.length());
	return (data.NextPos != 0);
}

void MenuText::AddBreak(ParseData& data, size_t from, size_t to)
{
	if (!data.IsBreak && (to - from) < 1)
		return;

	TextTiles.push_back(trim(TextString.substr(from, to - from)));

	data.LastBreak = to + (data.IsBreak ? 2 : 1);
	data.FirstWord = 0;
}

void MenuText::DeleteLastLetter()
{
	// Needs at least 1 character.
	if (!TextString.empty())
		SetText(TextString.substr(0, TextString.length() - 1));
}

void MenuText::Draw()
{
	if (!Visible)
		return;

	float X2, Y2;

	SetFontStyle(Style);
	SetFontSize(Size);
	SetFontItalic(false);

	glColorRGBInt(ColRGB, Alpha, Int);

	SetFontReflection(Reflection, ReflectionSpacing);

	// If selected, blink...
	if (IsSelected())
	{
		uint32 ticks = (uint32)((float) SDL_GetTicks() / 550.0f);
		if (ticks != STicks)
		{
			STicks = ticks;
			SelectBlink = !SelectBlink;
		}
	}

	// TODO: Temporary hack until I find why TextTiles are being cleared after being set.
	if (!TextString.empty() && TextTiles.empty())
		SetText(TextString);

    // draw text as many strings
	Y2 = Y + MoveY;
	for (std::vector<std::string>::iterator itr = TextTiles.begin(); itr != TextTiles.end(); ++itr)
	{
		std::string Text2 = (*itr);
		if (!(Selected && SelectBlink)
			|| ((itr + 1) != TextTiles.end()))
			;
        else
			Text2 += '|';

		switch (Align)
		{
		case 1:
			X2 = X + MoveX - glTextWidth(Text2) / 2; // centered
			break;

		case 2:
			X2  = X + MoveX - glTextWidth(Text2); // right aligned
			break;

		default:
			X2 = X + MoveX; // left aligned (default)
		}

		SetFontPos(X2, Y2);
		SetFontZ(Z);

		glPrint(Text2);

		if (Style == ftBold)
			Y2 = Y2 + Size * 0.93f;
		else
			Y2 = Y2 + Size * 0.72f;
	}

	SetFontStyle(ftNormal); // reset to default
}
