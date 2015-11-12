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
static const std::string FontNames[] =
{
	"Normal", "Bold", "Outline1", "Outline2", "BoldHighRes"
};

path FindFontFile(const std::string& filename)
{
	path result = FontPath / filename;
	// if path does not exist, try as an absolute path
	if (!boost::filesystem::exists(result))
		result = filename;

	return result;
}

void AddFontFallbacks(CSimpleIniA& ini, ScalableFont * font)
{
	const std::string baseKey = "File";
	// Evaluate the ini file's 'Fallbacks' section
	for (int i = 1; i <= 10; i++)
	{
		std::string key = baseKey + boost::lexical_cast<std::string>(i);
		std::string fontFilename = ini.GetValue("Fallbacks", key.c_str(), "");
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
			sLog.Error("AddFontFallbacks", ex.what());
		}
	}
}

// Build all fonts
void BuildFonts()
{
	ActiveFont = 0;
	Fonts.assign(SDL_arraysize(FontNames), GLFont());

	CSimpleIniA ini(true);
	path iniPath = FontPath / FONTS_FILE;
	SI_Error result = ini.LoadFile(iniPath.c_str());
	if (result != SI_OK)
		return sLog.Debug("BuildFonts", "Failed to load config.");

	try
	{
		for (size_t i = 0; i < Fonts.size(); i++)
		{
			GLFont& font = Fonts[i];

			const std::string section = "Font_" + FontNames[i];
			std::string fontFile = ini.GetValue(section.c_str(), "File", "");
			path fontPath = FindFontFile(fontFile);

			long fontMaxResolution = ini.GetLongValue(section.c_str(), "MaxResolution", 64);
			bool fontPrecache = ini.GetBoolValue(section.c_str(), "PreCache", true);
			float fontOutline = (float) ini.GetDoubleValue(section.c_str(), "Outline", 0.0);

			// Create either outlined or normal font
			if (fontOutline > 0.0f)
			{
				// outlined font
				FTScalableOutlineFont * outlineFont = new FTScalableOutlineFont(
					fontPath, (int) fontMaxResolution, fontOutline, true, fontPrecache);

				outlineFont->SetOutlineColor(
					(float) ini.GetDoubleValue(section.c_str(), "OutlineColorR", 0.0),
					(float) ini.GetDoubleValue(section.c_str(), "OutlineColorG", 0.0),
					(float) ini.GetDoubleValue(section.c_str(), "OutlineColorB", 0.0),
					(float) ini.GetDoubleValue(section.c_str(), "OutlineColorA", -1.0)
					);
				font.Font = outlineFont;
				font.Outlined = true;
			}
			else
			{
				float fontEmbolden = (float) ini.GetDoubleValue(section.c_str(), "Embolden", 0.0);
				font.Font = new FTScalableFont(
					fontPath, fontMaxResolution, fontEmbolden, true, fontPrecache);
				font.Outlined = false;
			}

			font.Font->GlyphSpacing = (float) ini.GetDoubleValue(section.c_str(), "GlyphSpacing", 0.0);
			font.Font->Stretch = (float) ini.GetDoubleValue(section.c_str(), "Stretch", 1.0);
			font.Font->Init();

			AddFontFallbacks(ini, font.Font);
		}
	}
	catch (FontException& ex)
	{
		sLog.Critical("BuildFonts", ex.what());
	}
}

// Deletes all fonts
void KillFonts()
{
	Fonts.clear();
}

// Returns text width
float glTextWidth(const std::string& text)
{
	FontBounds bounds = Fonts[ActiveFont].Font->BBox(text, true);
	return bounds.Right - bounds.Left;
}

// Custom OpenGL print routine
void glPrint(const char * format, ...)
{
	char buffer[1024];
	std::string text;
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, 1024, format, args);
	va_end(args);
	text = buffer;

	glPrint(text);
}

void glPrint(const std::string& text)
{
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
void SetFontStyle(Uint32 style)
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
