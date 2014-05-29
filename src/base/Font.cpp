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
#include "Font.h"

// shear factor used for the italic effect (bigger value -> more bending)
static const GLfloat cShearFactor = 0.25f;

static const GLfloat cShearMatrix[] =
{
	1, 0, 0, 0,
	cShearFactor, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

static const GLfloat cShearMatrixInv[] =
{
	1, 0, 0, 0,
	-cShearFactor, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

FontBase::FontBase()
{
	ResetIntern();
}

FontBase::FontBase(const path& filename)
{
	SetFilename(filename);
	ResetIntern();
}

void FontBase::SetFilename(const path& filename)
{
	Filename = filename;
}

void FontBase::ResetIntern()
{
	Style = 0;
	UseKerning = true;
	GlyphSpacing = 0.0f;

	// must be set by subclasses
	LineSpacing = 0.0f;
	ReflectionSpacing = 0.0f;
}

void FontBase::SplitLines(const tstring& string, LineArray& lines)
{
	StrSplit(string, _T("\r"), &lines);
}

void FontBase::PrintLines(const LineArray& lines, bool reflectionPass /*= false*/)
{
	// Recursively call this function to draw reflected text.
	if (!reflectionPass
		&& (Style & fsReflect))
		PrintLines(lines, true);

	// Store current colour, enable flags, matrix mode
	glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT);

	// Set OpenGL state
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef FLIP_YAXIS
	glPushMatrix();
	glScalef(1.0f, -1.0f, 1.0f);
#endif

	// Display text
	for (size_t lineIndex = 0; lineIndex < lines.size(); lineIndex++)
	{
		glPushMatrix();

		// Move to baseline
		glTranslatef(0.0f, -LineSpacing * lineIndex, 0.0f);

		// Draw underline
		if (!reflectionPass
			&& (Style & fsUnderline))
		{
			glDisable(GL_TEXTURE_2D);
			DrawUnderline(lines[lineIndex]);
			glEnable(GL_TEXTURE_2D);
		}

		// Draw reflection
		if (reflectionPass)
		{
			// Set reflection spacing
			glTranslatef(0.0f, -ReflectionSpacing, 0.0f);

			// Flip y-axis
			glScalef(1.0f, -1.0f, 1.0f);
		}

		// Shear for italic effect
		if (Style & fsItalic)
			glMultMatrixf((const GLfloat *)&cShearMatrix);

		// Render text line
		Render(lines[lineIndex]);

		glPopMatrix();
	}

	// Restore settings
#ifdef FLIP_YAXIS
	glPopMatrix();
#endif
	glPopAttrib();
}

void FontBase::Print(const tstring& text)
{
	LineArray lines;
	SplitLines(text, lines);
	PrintLines(lines);
}

void FontBase::DrawUnderline(const tstring& line)
{
	float	y1 = GetUnderlinePosition(),
			y2 = y1 + GetUnderlineThickness();
	FontBounds bounds = BBox(line, false);
	glRectf(bounds.Left, y1, bounds.Right, y2);
}

FontBounds FontBase::BBox(const tstring& text, bool advance /*= true*/)
{
	LineArray lines;
	SplitLines(text, lines);
	return BBoxLines(lines, advance);
}

FontBase::~FontBase()
{
}

ScalableFont::ScalableFont(bool useMipMaps)
	: FontBase()
{
	memset(MipmapFonts, 0, sizeof(MipmapFonts));
	UseMipmaps = useMipMaps;
	ResetIntern();
}

void ScalableFont::SetBaseFont(FontBase * baseFont)
{
	BaseFont = baseFont;
	MipmapFonts[0] = baseFont;
}

void ScalableFont::Init()
{
	SetFilename(BaseFont->Filename);

	// Create mipmap fonts if requested
	if (UseMipmaps)
	{
		for (int mipmapLevel = 1; mipmapLevel < MaxMipmapLevel; mipmapLevel++)
		{
			MipmapFonts[mipmapLevel] = CreateMipmap(mipmapLevel, 1.0f / (1 << mipmapLevel));

			// Stop if no smaller mipmap font is returned
			if (MipmapFonts[mipmapLevel] == NULL)
				break;
		}
	}
}

void ScalableFont::ResetIntern()
{
	Scale = 1.0f;
	Stretch = 1.0f;
}

FontBounds ScalableFont::BBoxLines(const LineArray& lines, bool advance)
{
	FontBounds result = BaseFont->BBoxLines(lines, advance);
	result.Left *= Scale * Stretch;
	result.Right *= Scale * Stretch;
	result.Top *= Scale;
	result.Bottom *= Scale;
	return result;
}

void ScalableFont::PrintLines(const LineArray& lines, bool reflectionPass /*= false*/)
{
	glPushMatrix();

	// set scale and stretching
	glScalef(Scale * Stretch, Scale, 0.0f);

	// print text
#if 0 // TODO
	if (UseMipmaps)
		ChooseMipmapFont().PrintLines(lines)
	else
#endif
		BaseFont->PrintLines(lines);

	glPopMatrix();
}

void ScalableFont::Render(const tstring& text)
{
	assert(!"Unused ScalableFont::Render() called.");
}

float ScalableFont::GetUnderlinePosition()
{
	return BaseFont->GetUnderlinePosition();
}

float ScalableFont::GetUnderlineThickness()
{
	return BaseFont->GetUnderlineThickness();
}

void ScalableFont::SetHeight(float height)
{
	Scale = height / BaseFont->GetHeight();
}

float ScalableFont::GetHeight()
{
	return BaseFont->GetHeight() * Scale;
}

float ScalableFont::GetAscender()
{
	return BaseFont->GetAscender() * Scale;
}

float ScalableFont::GetDescender()
{
	return BaseFont->GetDescender() * Scale;
}

ScalableFont::~ScalableFont()
{
	delete BaseFont;
}

FTScalableFont::FTScalableFont(const path& filename,
	int size, float outsetAmount /*= 0.0f*/, bool useMipmaps /*= true*/,
	bool preCache /*= true*/) : ScalableFont(useMipmaps)
{
	uint32 loadFlags = FT_LOAD_DEFAULT;
	if (useMipmaps)
		loadFlags |= FT_LOAD_NO_HINTING;

	FontBase * baseFont = new FTFont(filename, size, size * outsetAmount, preCache, loadFlags);
	SetBaseFont(baseFont);
}

void FTScalableFont::AddFallback(const path& filename)
{
	for (int i = 0; i < SDL_arraysize(MipmapFonts); i++)
	{
		FontBase * font = MipmapFonts[i];
		if (font != NULL)
			static_cast<FTFont *>(font)->AddFallback(filename);
	}
}

FontBase * FTScalableFont::CreateMipmap(int level, float scale)
{
	FTFont * baseFont = static_cast<FTFont *>(BaseFont);
	FTFont * result = NULL;

	int scaledSize = (int) Round(baseFont->Size * scale);
	if (scaledSize < 8)
		return NULL;

	result = new FTFont(baseFont->Filename, scaledSize, 
		baseFont->Outset * scale, baseFont->PreCache, FT_LOAD_DEFAULT | FT_LOAD_NO_HINTING);
	result->Init();
	return result;
}

GlyphCacheHashEntry::~GlyphCacheHashEntry()
{
	for (std::map<uint8, Glyph *>::iterator itr = GlyphTable.begin();
		itr != GlyphTable.end(); ++itr)
		delete itr->second;

	GlyphTable.clear();
}

CachedFont::CachedFont(const path& filename)
	: FontBase(filename)
{
	// TODO
}

FTFontFace::FTFontFace()
{
}

FTFontFace::FTFontFace(const path& filename)
	: Filename(filename)
{
}

FTOutlineFont::FTOutlineFont(const path& filename, int size, float outset,
	bool preCache /*= true*/, uint32 loadFlags /*= FT_LOAD_DEFAULT*/)
	: FontBase(filename), InnerFont(NULL), OutlineFont(NULL)
{
	Size = size;
	Outset = outset;

	InnerFont = new FTFont(filename, size, 0.0f, preCache, loadFlags);
	InnerFont->Part = fpInner;

	OutlineFont = new FTFont(filename, size, outset, preCache, loadFlags);
	OutlineFont->Part = fpOutline;

	ResetIntern();
}

void FTOutlineFont::ResetIntern()
{
	// TODO: maybe swap fInnerFont/fOutlineFont.GlyphSpacing to use the spacing
	// of the outline font?
	// InnerFont->GlyphSpacing = Outset*2;
	OutlineFont->GlyphSpacing = -Outset*2;

	LineSpacing = OutlineFont->LineSpacing;
	ReflectionSpacing = OutlineFont->ReflectionSpacing;

	OutlineColor.R = OutlineColor.G = OutlineColor.B = 0.0f;
	OutlineColor.A = -1.0f;
}

void FTOutlineFont::AddFallback(const path& filename)
{
	OutlineFont->AddFallback(filename);
	InnerFont->AddFallback(filename);
}

void FTOutlineFont::DrawUnderline(const tstring& line)
{
	GLColor currentColor, outlineColor;

	// save current color
	glGetFloatv(GL_CURRENT_COLOR, currentColor.vals);

	// if the outline's alpha component is < 0 use the current alpha
	outlineColor = OutlineColor;
	if (outlineColor.A < 0.0f)
		outlineColor.A = currentColor.A;

	// draw underline outline (in outline color)
	glColor4fv(outlineColor.vals);
	glPushMatrix();
	OutlineFont->DrawUnderline(line);
	glPopMatrix();

	// draw underline inner part (in current color)
	glColor4fv(currentColor.vals);
	glTranslatef(Outset, 0.0f, 0.0f);
	glPushMatrix();
	InnerFont->DrawUnderline(line);
	glPopMatrix();
}

void FTOutlineFont::Render(const tstring& line)
{
	GLColor currentColor, outlineColor;

	// save current color
	glGetFloatv(GL_CURRENT_COLOR, currentColor.vals);

	// if the outline's alpha component is < 0 use the current alpha
	outlineColor = OutlineColor;
	if (outlineColor.A < 0.0f)
		outlineColor.A = currentColor.A;

	// setup and render outline font
	glColor4fv(outlineColor.vals);
	glPushMatrix();
	OutlineFont->Render(line);
	glPopMatrix();

	// setup and render inner font
	glColor4fv(currentColor.vals);
	glTranslatef(Outset, Outset, 0.0f);
	glPushMatrix();
	InnerFont->Render(line);
	glPopMatrix();
}

FontBounds FTOutlineFont::BBoxLines(const LineArray& lines, bool advance)
{
	return OutlineFont->BBoxLines(lines, advance);
}

float FTOutlineFont::GetUnderlinePosition()
{
	return OutlineFont->GetUnderlinePosition();
}

float FTOutlineFont::GetUnderlineThickness()
{
	return OutlineFont->GetUnderlineThickness();
}

float FTOutlineFont::GetHeight()
{
	return OutlineFont->GetHeight();
}

float FTOutlineFont::GetAscender()
{
	return OutlineFont->GetAscender();
}

float FTOutlineFont::GetDescender()
{
	return OutlineFont->GetDescender();
}

FTOutlineFont::~FTOutlineFont()
{
	delete InnerFont;
	delete OutlineFont;
}

FTScalableOutlineFont::FTScalableOutlineFont(const path& filename, int size,
	float outsetAmount, bool useMipmaps /*= true*/, bool preCache /*= true*/)
	: ScalableFont(useMipmaps)
{
	uint32 loadFlags = FT_LOAD_DEFAULT;

	// Disable hinting and grid-fitting (see ScalableFont::Create)
	if (useMipmaps)
		loadFlags |= FT_LOAD_NO_HINTING;

	FontBase * font = new FTOutlineFont(filename, size, size * outsetAmount, preCache, loadFlags);
	font->Init();
	SetBaseFont(font);
}

FontBase * FTScalableOutlineFont::CreateMipmap(int level, float scale)
{
	FTOutlineFont * baseFont = static_cast<FTOutlineFont *>(BaseFont);
	FTOutlineFont * result = NULL;

	int scaledSize = (int) Round(baseFont->Size * scale);
	if (scaledSize < 8)
		return NULL;

	result = new FTOutlineFont(baseFont->Filename, scaledSize, 
		baseFont->Outset * scale, baseFont->PreCache, FT_LOAD_DEFAULT | FT_LOAD_NO_HINTING);
	result->Init();
	return result;
}

void FTScalableOutlineFont::AddFallback(const path& filename)
{
	for (int i = 0; i < SDL_arraysize(MipmapFonts); i++)
	{
		FontBase * font = MipmapFonts[i];
		if (font != NULL)
			static_cast<FTOutlineFont *>(font)->AddFallback(filename);
	}
}

void FTScalableOutlineFont::SetOutlineColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a /*= -1.0f*/)
{
	// TODO
}

void FTScalableOutlineFont::FlushCache(bool keepBaseSet)
{
	// TODO
}

float FTScalableOutlineFont::GetOutset()
{
	return static_cast<FTOutlineFont *>(BaseFont)->Outset * Scale;
}

FTFont::FTFont(const path& filename,
	int size, float outset /*= 0.0f*/, bool preCache /*= true*/,
	uint32 loadFlags /*= FT_LOAD_DEFAULT*/) : CachedFont(filename), Face(filename)
{
	Size = size;
	Outset = outset;
	PreCache = preCache;
	LoadFlags = loadFlags;
	UseDisplayLists = true;
	Part = 0;
	// TODO
	// Face = GetFaceCache().LoadFace(filename, size);

	ResetIntern();

	// pre-cache some commonly used glyphs (' ' - '~')
	if (preCache)
	{
// TODO
//		for (char ch = ' '; ch < '~'; ch++)
//			Cache.AddGlyph(ch, new FTGlyph(this, ch, outset, loadFlags)); 
	}
}

FontBounds FTFont::BBoxLines(const LineArray& lines, bool advance)
{
	FontBounds result = { 0.0f, 0.0f, 0.0f, 0.0f };
	// TODO
	return result;
}

void FTFont::AddFallback(const path& filename)
{
	FTFontFace fontFace;
	// GetFaceCache().LoadFace(filename, Size, &fontFace);
	FallbackFaces.push_back(fontFace);
}

float FTFont::GetUnderlinePosition()
{
	// TODO
	return 0.0f;
}

float FTFont::GetUnderlineThickness()
{
	// TODO
	return 0.0f;
}

float FTFont::GetHeight()
{
	return GetAscender() - GetDescender();
}

float FTFont::GetAscender()
{
	return Face.Face.ascender * Face.FontUnitScale.Y + Outset*2;
}

float FTFont::GetDescender()
{
	// Note: outset is not part of the descender as the baseline is lifted
	return Face.Face.descender * Face.FontUnitScale.Y;
}

void FTFont::Render(const tstring& text)
{
	// TODO
}
