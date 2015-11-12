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
#include "Menu.h"
#include "../base/TextGL.h"

static const int ArrowAlphaOptionsLeft = 1;
static const int ArrowAlphaNoOptionsLeft = 0;
static const int MinItemSpacing = 5;
static const int MinSideSpacing = 24;

MenuSelectSlide::MenuSelectSlide() : pData(NULL)
{
	Visible = true;

	TextOptions.push_back(MenuText());

	Colorized = ColorizedSBG = false;

	Int = ColRGB.R = ColRGB.G = ColRGB.B = 1.0f;
	DInt = DColRGB.R = DColRGB.G = DColRGB.B = 1.0f;
	TInt = TColRGB.R = TColRGB.G = TColRGB.B = 1.0f;
	TDInt = TDColRGB.R = TDColRGB.G = TDColRGB.B = 1.0f;
	STInt = STColRGB.R = STColRGB.G = STColRGB.B = 1.0f;
	STDInt = STDColRGB.R = STDColRGB.G = STDColRGB.B = 1.0f;
	SBGInt = SBGColRGB.R = SBGColRGB.G = SBGColRGB.B = 1.0f;
	SBGDInt = SBGDColRGB.R = SBGDColRGB.G = SBGDColRGB.B = 1.0f;

	ShowArrows = false;
	OneItemOnly = false;

	Lines = 0;
}

void MenuSelectSlide::SetSelected(bool value)
{
	Selected = value;

	if (value)
	{
		Tex.ColRGB = ColRGB;
		Tex.Int = Int;

		Text.ColRGB = TColRGB;
		Text.Int = TInt;

		TexSBG.ColRGB = SBGColRGB;
		TexSBG.Int = SBGInt;
	}
	else
	{
		if (Colorized)
		{
			DeselectTexture.Int = DInt;
		}
		else
		{
			Tex.ColRGB = DColRGB;
			Tex.Int = DInt;
		}

		Text.ColRGB = TDColRGB;
		Text.Int = TDInt;

		if (ColorizedSBG)
		{
			DeselectTextureSBG.Int = SBGDInt;
		}
		else
		{
			TexSBG.ColRGB = SBGDColRGB;
			TexSBG.Int = SBGDInt;
		}
	}
}

void MenuSelectSlide::SetSelectOpt(Uint32 value)
{
	if (TextOptions.empty()
		|| TextOptionNames.empty())
		return;

	*pData = value;

	// First option selected
	if (value <= 0)
	{
		Tex_SelectS_ArrowL.Alpha = ArrowAlphaNoOptionsLeft;
		Tex_SelectS_ArrowR.Alpha = (float) (TextOptionNames.size() > 1 ? ArrowAlphaOptionsLeft : ArrowAlphaNoOptionsLeft);

		size_t i = 0;
		for (std::vector<MenuText>::iterator itr = TextOptions.begin(); itr != TextOptions.end(); ++itr, ++i)
			(*itr).SetText(AdjustOptionTextToFit(TextOptionNames[i]));

		DoSelection(0);
	}
	// Last option selected
	else if (value >= TextOptionNames.size() - 1)
	{
		Tex_SelectS_ArrowL.Alpha = ArrowAlphaOptionsLeft;
		Tex_SelectS_ArrowR.Alpha = ArrowAlphaNoOptionsLeft;

		size_t i = TextOptions.size() - 1;
		for (std::vector<MenuText>::reverse_iterator itr = TextOptions.rbegin(); itr != TextOptions.rend(); ++itr, --i)
			(*itr).SetText(AdjustOptionTextToFit(TextOptionNames[TextOptionNames.size() - Lines - i]));

		DoSelection(Lines - 1);
	}
	// In between first and last
	else
	{
		Tex_SelectS_ArrowL.Alpha = ArrowAlphaOptionsLeft;
		Tex_SelectS_ArrowR.Alpha = ArrowAlphaOptionsLeft;

		Uint32 halfL = (Uint32) std::ceil((Lines - 1) / 2.0f);
		Uint32 halfR = Lines - 1 - halfL;

		// Select option is near to the left side
		if (value <= halfL)
		{
			size_t i = 0;
			for (std::vector<MenuText>::iterator itr = TextOptions.begin(); itr != TextOptions.end(); ++itr, ++i)
				(*itr).SetText(AdjustOptionTextToFit(TextOptionNames[i]));
			DoSelection(value);
		}
		// Selected option is near to the right side
		else if (value > TextOptionNames.size() - halfR)
		{
			halfL = TextOptionNames.size() - value;
			halfR = Lines - 1 - halfR;

			size_t i = 0;
			for (std::vector<MenuText>::reverse_iterator itr = TextOptions.rbegin(); itr != TextOptions.rend(); ++itr, ++i)
				(*itr).SetText(AdjustOptionTextToFit(TextOptionNames[TextOptionNames.size() - (Lines - i - 1)]));

			DoSelection(halfL);
		}
		else
		{
			size_t i = 0;
			for (std::vector<MenuText>::iterator itr = TextOptions.begin(); itr != TextOptions.end(); ++itr, ++i)
				(*itr).SetText(AdjustOptionTextToFit(TextOptionNames[value - halfL + i]));
			DoSelection(halfL);
		}
	}
}

void MenuSelectSlide::DoSelection(size_t sel)
{
	for (std::vector<MenuText>::iterator itr = TextOptions.begin(); itr != TextOptions.end(); ++itr)
	{
		(*itr).ColRGB = STDColRGB;
		(*itr).Int = STDInt;
	}

	if (sel < TextOptions.size())
	{
		MenuText& opt = TextOptions[sel];
		opt.ColRGB = STColRGB;
		opt.Int = STInt;
	}
}

void MenuSelectSlide::Draw()
{
	if (!Visible)
		return;

	if (Selected || !Colorized)
	{
		Tex.Draw();
	}
	else
	{
		DeselectTexture.X = Tex.X;
		DeselectTexture.Y = Tex.Y;
		DeselectTexture.W = Tex.W;
		DeselectTexture.H = Tex.H;
		DeselectTexture.Draw();
	}

	if (Selected || !ColorizedSBG)
	{
		TexSBG.Draw();
	}
	else
	{
		DeselectTextureSBG.X = TexSBG.X;
		DeselectTextureSBG.Y = TexSBG.Y;
		DeselectTextureSBG.W = TexSBG.W;
		DeselectTextureSBG.H = TexSBG.H;
		DeselectTextureSBG.Draw();
	}

	if (ShowArrows)
	{
		Tex_SelectS_ArrowL.Draw();
		Tex_SelectS_ArrowR.Draw();
	}

	Text.Draw();
	for (std::vector<MenuText>::iterator itr = TextOptions.begin(); itr != TextOptions.end(); ++itr)
		(*itr).Draw();
}

void MenuSelectSlide::GenerateLines()
{
	float maxLen = 0.0f;

	SetFontStyle(ftNormal); /* Text.Style */
	SetFontSize(Text.Size);

	for (std::vector<std::string>::const_iterator itr = TextOptionNames.begin(); itr != TextOptionNames.end(); ++itr)
	{
		float len = glTextWidth(*itr);
		if (len > maxLen)
			maxLen = len;
	}

	// Show one item only
	if (OneItemOnly)
	{
		Lines = 1;
	}
	// Show all items
	else
	{
		Lines = (Uint8) std::floor((TexSBG.W - MinSideSpacing * 2) / (maxLen + MinItemSpacing));
		if (Lines > TextOptionNames.size())
			Lines = (Uint8) TextOptionNames.size();

		if (Lines <= 0)
			Lines = 1;
	}

	// Free old space used by texts
	TextOptions.clear();

	for (Uint8 i = 0; i < Lines; i++)
	{
		MenuText text;

		text.Size = Text.Size;
		text.Visible = true;
		text.Style = 0;

		text.ColRGB = STDColRGB;
		text.Int = STDInt;

		// Generate positions
		text.Y = TexSBG.Y + (TexSBG.H - Text.Size) / 2;
		text.X = TexSBG.X;

		text.Align = 0;

		// Better look with 2 options & a single option
		switch (Lines)
		{
		case 2:
			assert(TextOptionNames.size() >= 2);
			text.X += 20 + (TexSBG.W - 40 - glTextWidth(TextOptionNames[1])) * i;
			break;

		case 1:
			text.X += (TexSBG.W / 2);
			text.Align = 1; // center
			break;

		default:
			text.X += (TexSBG.W / 2) + (TexSBG.W - MinSideSpacing * 2) * ((float)i / (float)Lines - 0.5f);
			break;
		}

		TextOptions.push_back(text);
	}
}

MouseOverRect MenuSelectSlide::GetMouseOverRect()
{
	MouseOverRect rect = {0};

	rect.X = Tex.X;
	rect.Y = Tex.Y;
	rect.W = (TexSBG.X + TexSBG.W) - Tex.X;
	rect.H = std::max(Tex.H, TexSBG.H);

	return rect;
}

MouseClickAction MenuSelectSlide::OnClick(float x, float y)
{
	// Use left sides to inc or dec selection by click
	float areaW = TexSBG.W / 20.0f;

	if (y > TexSBG.Y && y <= TexSBG.Y + TexSBG.H)
	{
		if (x >= TexSBG.X && x <= TexSBG.X + areaW)
			return maLeft; // hit left area
		else if (x >= TexSBG.X + TexSBG.W - areaW && x <= TexSBG.X + TexSBG.W)
			return maRight; // hit right area
	}

	return maReturn;
}

// Cuts the text if it is too long to fit on the select background
std::string MenuSelectSlide::AdjustOptionTextToFit(const std::string& optionText)
{
	std::string result = optionText;
	float maxLen;
	size_t len;

	if (TexSBG.W > 0)
	{
		maxLen = TexSBG.W - MinSideSpacing * 2;

		SetFontStyle(ftNormal);
		SetFontSize(Text.Size);

		// We will remove min. 2 letters by default and replace them with points
		// if the whole text doesn't fit
		len = result.length() - 1;

		while (len > 0 && glTextWidth(result) > maxLen)
		{
			// ensure that we only cut at full letters
			// this may be a problem if there is text which consists 
			// of many multibyte chars and only few one byte chars.
			do
			{
				--len;
			} while  ((optionText[len] & 0x80) == 0);

			result = optionText.substr(0, len) + "..";
		}
	}

	return result;
}

