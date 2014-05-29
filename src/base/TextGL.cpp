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
#include "Font.h"
#include "PathUtils.h"
#include "Log.h"

static size_t ActiveFont;
static std::vector<GLFont> Fonts;
static const tstring FontNames[] =
{
	_T("Normal"), _T("Bold"), _T("Outline1"), _T("Outline2"), _T("BoldHighRes")
};

path FindFontFile(const tstring& filename)
{
	path result = FontPath / filename;
	// if path does not exist, try as an absolute path
	if (!boost::filesystem::exists(result))
		result = filename;

	return result;
}

void AddFontFallbacks(CSimpleIni& ini, ScalableFont * font)
{
	const tstring baseKey = _T("File");
	// Evaluate the ini file's 'Fallbacks' section
	for (int i = 1; i <= 10; i++)
	{
		tstring key = baseKey + boost::lexical_cast<tstring>(i);
		tstring fontFilename = ini.GetValue(_T("Fallbacks"), key.c_str(), _T(""));
		if (fontFilename.empty())
			continue;

		path fontPath = FindFontFile(fontFilename);
		if (!boost::filesystem::exists(fontPath))
			continue;

		try
		{
			font->AddFallback(fontPath);
		}
		catch (FontException& ex)
		{
			sLog.Error(_T("AddFontFallbacks"), ex.twhat());
		}
	}
}

// Build all fonts
void BuildFonts()
{
	ActiveFont = 0;
	Fonts.assign(SDL_arraysize(FontNames), GLFont());

	CSimpleIni ini(true);
	path iniPath = FontPath / FONTS_FILE;
	SI_Error result = ini.LoadFile(iniPath.c_str());
	if (result != SI_OK)
		return sLog.Debug(_T("BuildFonts"), _T("Failed to load config."));

	try
	{
		for (size_t i = 0; i < Fonts.size(); i++)
		{
			GLFont& font = Fonts[i];

			const tstring section = _T("Font_") + FontNames[i];
			tstring fontFile = ini.GetValue(section.c_str(), _T("File"), _T(""));
			path fontPath = FindFontFile(fontFile);

			long fontMaxResolution = ini.GetLongValue(section.c_str(), _T("MaxResolution"), 64);
			bool fontPrecache = ini.GetBoolValue(section.c_str(), _T("PreCache"), true);
			float fontOutline = (float) ini.GetDoubleValue(section.c_str(), _T("Outline"), 0.0);

			// Create either outlined or normal font
			if (fontOutline > 0.0f)
			{
				// outlined font
				FTScalableOutlineFont * outlineFont = new FTScalableOutlineFont(
					fontPath, (int) fontMaxResolution, fontOutline, true, fontPrecache);

				outlineFont->SetOutlineColor(
					(float) ini.GetDoubleValue(section.c_str(), _T("OutlineColorR"), 0.0),
					(float) ini.GetDoubleValue(section.c_str(), _T("OutlineColorG"), 0.0),
					(float) ini.GetDoubleValue(section.c_str(), _T("OutlineColorB"), 0.0),
					(float) ini.GetDoubleValue(section.c_str(), _T("OutlineColorA"), -1.0)
					);
				font.Font = outlineFont;
				font.Outlined = true;
			}
			else
			{
				float fontEmbolden = (float) ini.GetDoubleValue(section.c_str(), _T("Embolden"), 0.0);
				font.Font = new FTScalableFont(
					fontPath, fontMaxResolution, fontEmbolden, true, fontPrecache);
				font.Outlined = false;
			}

			font.Font->GlyphSpacing = (float) ini.GetDoubleValue(section.c_str(), _T("GlyphSpacing"), 0.0);
			font.Font->Stretch = (float) ini.GetDoubleValue(section.c_str(), _T("Stretch"), 1.0);
			font.Font->Init();

			AddFontFallbacks(ini, font.Font);
		}
	}
	catch (FontException& ex)
	{
		sLog.Critical(_T("BuildFonts"), ex.twhat());
	}
}

// Deletes all fonts
void KillFonts()
{
	Fonts.clear();
}

// Returns text width
float glTextWidth(const tstring& text)
{
	FontBounds bounds = Fonts[ActiveFont].Font->BBox(text, true);
	return bounds.Right - bounds.Left;
}

// Custom OpenGL print routine
void glPrint(const tstring& text, ...)
{
	if (text.empty())
		return;

	GLFont& font = Fonts[ActiveFont];
	glPushMatrix();
		// Set font position
		glTranslatef(font.X, font.Y + font.Font->GetAscender(), font.Z);

		// Draw string
		font.Font->Print(text);
	glPopMatrix();
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
	Fonts[ActiveFont].X = X;
	Fonts[ActiveFont].Y = Y;
}

// Sets Z position of float
void SetFontZ(float Z)
{
	Fonts[ActiveFont].Z = Z;
}

// Sets font size
void SetFontSize(float height)
{
	Fonts[ActiveFont].Font->SetHeight(height);
}

// Sets active font style (normal, bold, etc)
void SetFontStyle(uint32 style)
{
	if (style >= Fonts.size())
		return;

	ActiveFont = style;
}

// Enables/disables italics.
void SetFontItalic(bool enable)
{
	GLFont& font = Fonts[ActiveFont];
	if (enable)
		font.Font->Style |= fsItalic;
	else
		font.Font->Style &= ~fsItalic;
}

// Enables/disables text reflection.
void SetFontReflection(bool enable, float spacing)
{
	GLFont& font = Fonts[ActiveFont];

	if (enable)
		font.Font->Style |= fsReflect;
	else
		font.Font->Style &= ~fsReflect;

	font.Font->ReflectionSpacing = spacing + font.Font->GetDescender();
}
