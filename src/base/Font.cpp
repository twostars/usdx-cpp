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
#include "Log.h"

static FreeType s_ftLibrary;

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

FTFontFaceCache FTFont::s_fontFaceCache;

/**
* Size of the transparent border surrounding the glyph image in the texture.
* The border is necessary because OpenGL does not smooth texels at the
* border of a texture with the GL_CLAMP or GL_CLAMP_TO_EDGE styles.
* Without the border, magnified glyph textures look very ugly at their edges.
* It looks edgy, as if some pixels are missing especially on the left edge
* (just set cTexSmoothBorder to 0 to see what is meant by this).
* With the border even the glyphs edges are blended to the border (transparent)
* color and everything looks nice.
*
* Note:
* OpenGL already supports texture border by setting the border parameter
* of glTexImage*D() to 1 and using a texture size of 2^m+2b and setting the
* border pixels to the border color. In some forums it is discouraged to use
* the border parameter as only a few of the more modern graphics cards support
* this feature. On an ATI Radeon 9700 card, the slowed down to 0.5 fps and
* the glyph's background got black. So instead of using this feature we
* handle it on our own. The only drawback is that textures might get bigger
* because the border might require a higher power of 2 size instead of just
* two additional pixels.
*/
const int cTexSmoothBorder = 1;

INLINE int NextPowerOf2(int value)
{
	int result = 1;
	while (result < value)
		result <<= 1;
	return result;
}

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

void FontBase::SplitLines(const std::string& string, LineArray& lines)
{
	StrSplit(string, "\r", &lines);
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
		Render(lines[lineIndex], reflectionPass);

		glPopMatrix();
	}

	// Restore settings
#ifdef FLIP_YAXIS
	glPopMatrix();
#endif
	glPopAttrib();
}

void FontBase::Print(const std::string& text)
{
	LineArray lines;
	SplitLines(text, lines);
	PrintLines(lines);
}

void FontBase::DrawUnderline(const std::string& line)
{
	float	y1 = GetUnderlinePosition(),
			y2 = y1 + GetUnderlineThickness();
	FontBounds bounds = BBox(line, false);
	glRectf(bounds.Left, y1, bounds.Right, y2);
}

FontBounds FontBase::BBox(const std::string& text, bool advance /*= true*/)
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

/**
 * Returns the correct mipmap font for the current scale and projection
 * matrix. The modelview scale is adjusted to the mipmap level, so
 * Print() will display the font in the correct size.
 */
FontBase * ScalableFont::ChooseMipmapFont()
{
	FontBase * result = NULL;
	int desiredLevel = GetMipmapLevel();

	// get the smallest mipmap available for the desired level
	// as not all levels must be assigned to a font.
	for (int level = desiredLevel; level >= 0; level--)
	{
		FontBase * font = MipmapFonts[level];
		if (font != NULL)
		{
			result = font;
			break;
		}
	}

	if (result == NULL)
		sLog.Critical("ScalableFont::ChooseMipmapFont", "No mipmap font available for desired level %d. Base font is '%s'.",
		desiredLevel, BaseFont->Filename.c_str());

	// since the mipmap font (if level > 0) is smaller than the base-font
	// we have to scale to get its size right.
	float MipmapScale = MipmapFonts[0]->GetHeight() / result->GetHeight();
	glScalef(MipmapScale, MipmapScale, 0);

	return result;
}

/**
 * Returns the mipmap level to use with regard to the current projection
 * and modelview matrix, font scale and stretch.
 *
 * Note:
 * - for Freetype fonts, hinting and grid-fitting must be disabled, otherwise
 *   the glyph widths/heights ratios and advance widths of the mipmap fonts
 *   do not match as they are adjusted sligthly (e.g. an 'a' at size 12px has
 *   width 12px, but at size 6px width 8px).
 * - returned mipmap-level is used for all glyphs of the current text to print.
 *   This is faster, much easier to handle, since we just need to create
 *   multiple sized fonts and select the one we need for the mipmap-level and
 *   it avoids that neighbored glyphs use different mipmap-level which might
 *   look odd because one glyph might look blurry and the other sharp.
 *
 * Motivation:
 *   We do not use OpenGL for mipmapping as the results are very bad. At least
 *   with automatic mipmap generation (gluBuildMipmaps) the fonts look rather
 *   blurry.
 *   Defining our own mipmaps by creating multiple textures with
 *   for different mimap levels is a pain, as the font size passed to freetype
 *   is not the size of the bitmaps created and it does not guarantee that a
 *   glyph bitmap of a font with font-size s/2 is half the size of the font with
 *   font-size s. If the bitmap size is just a single pixel bigger than the half
 *   we might need a texture of the next power-of-2 and the texture would not be
 *   half of the size of the next bigger mipmap. In addition we use a fixed one
 *   pixel sized border to smooth the texture (see cTexSmoothBorder) and maybe
 *   an outset that is added to the font, so creating a glyph mipmap that is
 *   exactly half the size of the next bigger one is a very difficult task.
 *
 * Solution:
 *   Use mipmap textures that are not exactly half the size of the next mipmap
 *   level. OpenGL does not support this (at least not without extensions).
 *   The trickiest task is to determine the mipmap to use by calculating the
 *   amount of minification that is performed in this function.
 */
int ScalableFont::GetMipmapLevel()
{
	// width/height of square used for determining the scale
	const GLdouble cTestSize = 10.0;

	// an offset to the mipmap-level to adjust the change-over of two consecutive
	// mipmap levels. If for example the bias is 0.1 and unbiased level is 1.9
	// the result level will be 2. A bias of 0.5 is equal to rounding.
	// With bias=0.1 we prefer larger mipmaps over smaller ones.
	const GLdouble cBias = 0.2;

	GLdouble ModelMatrix[16], ProjMatrix[16];
	GLint ViewportArray[4];
	GLdouble WinCoords[3][3] =
	{
		{ 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0 },
	};
	double Dist, Dist2, DistSum, WidthScale, HeightScale;

	// 1. Retrieve current transformation matrices for gluProject
	glGetDoublev(GL_MODELVIEW_MATRIX, ModelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, ProjMatrix);
	glGetIntegerv(GL_VIEWPORT, ViewportArray);

	// 2. Project 3 of the corner points of a square with size cTestSize
	// to window coordinates (the square is just a dummy for a glyph).

	// project point (x1, y1) to window coordinates
	gluProject(0.0, 0.0, 0.0,
		ModelMatrix, ProjMatrix, ViewportArray,
		&WinCoords[0][0], &WinCoords[0][1], &WinCoords[0][2]);

	// project point (x2, y1) to window coordinates
	gluProject(cTestSize, 0.0, 0.0,
		ModelMatrix, ProjMatrix, ViewportArray,
		&WinCoords[1][0], &WinCoords[1][1], &WinCoords[1][2]);

	// project point (x1, y2) to window coordinates
	gluProject(0.0, cTestSize, 0.0,
		ModelMatrix, ProjMatrix, ViewportArray,
		&WinCoords[2][0], &WinCoords[2][1], &WinCoords[2][2]);

	// 3. Lets see how much the width and height of the square changed.
	// Calculate the width and height as displayed on the screen in window
	// coordinates and calculate the ratio to the original coordinates in
	// modelview space so the ratio gives us the scale (minification here).

	// projected width ||(x1, y1) - (x2, y1)||
	Dist = (WinCoords[0][0] - WinCoords[1][0]);
	Dist2 = (WinCoords[0][1] - WinCoords[1][1]);

	WidthScale = 1.0;
	DistSum = (Dist*Dist) + (Dist2*Dist2);
	if (DistSum > 0.0)
		WidthScale = cTestSize / std::sqrt(DistSum);

	// projected height ||(x1, y1) - (x2, y1)||
	Dist = (WinCoords[0][0] - WinCoords[2][0]);
	Dist2 = (WinCoords[0][1] - WinCoords[2][1]);

	HeightScale = 1.0;
	DistSum = (Dist*Dist) + (Dist2*Dist2);
	if (DistSum > 0.0)
		HeightScale = cTestSize / std::sqrt(DistSum);

	// 4. Now that we have got the scale, take the bigger minification scale
	// and get it to a logarithmic scale as each mipmap is 1/2 the size of its
	// predecessor (Mipmap_size[i] = Mipmap_size[i-1]/2).
	// The result is our mipmap-level = the index of the mipmap to use.

	// Level > 0: Minifaction; level < 0: Magnification
	int mipmapLevel = (int) Log2(std::max(WidthScale, HeightScale) + cBias);

	// Clamp to valid range
	if (mipmapLevel < 0)
		mipmapLevel = 0;
	else if (mipmapLevel >= SDL_arraysize(MipmapFonts))
		mipmapLevel = SDL_arraysize(MipmapFonts) - 1;

	return mipmapLevel;
}

void ScalableFont::PrintLines(const LineArray& lines, bool reflectionPass /*= false*/)
{
	glPushMatrix();

	// set scale and stretching
	glScalef(Scale * Stretch, Scale, 0.0f);

	// print text
	if (UseMipmaps)
		ChooseMipmapFont()->PrintLines(lines);
	else
		BaseFont->PrintLines(lines);

	glPopMatrix();
}

void ScalableFont::Render(const std::string& text, bool reflectionPass)
{
	sLog.Critical("ScalableFont::Render", "Unused method called. This should not be called..");
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
	for (std::map<uint8, Glyph *>::iterator itr = Table.begin(); itr != Table.end(); ++itr)
		delete itr->second;

	Table.clear();
}

FTFontFace * FTFontFaceCache::LoadFace(const path& filename, int size)
{
	FTFontFace * face = NULL;

#ifdef ENABLE_FT_FACE_CACHE
	for (std::set<FTFontFace *>::iterator itr = Faces.begin(); 
		itr != Faces.end(); ++itr)
	{
		face = *itr;
		if (face->Filename == filename
			&& face->Size == size)
		{
			face->IncRef();
			return face;
		}
	}
#endif

	face = new FTFontFace(filename, size);

	try
	{
		face->Load();
	}
	catch (FontException)
	{
		// Perform cleanup first to ensure we don't leak memory.
		delete face;
		face = NULL;

		// Then re-throw the exception.
		throw;
	}

	// Add reference as it's going in the set.
	face->IncRef();
	Faces.insert(face);

	return face;
}

void FTFontFaceCache::UnloadFace(FTFontFace * face)
{
	if (face == NULL)
		return;

	std::set<FTFontFace *>::iterator itr = Faces.find(face);
	if (itr != Faces.end()
		&& (*itr)->DecRef() == 0)
		Faces.erase(itr);
}

FTGlyph::FTGlyph(FTFont * font, char ch, float outset, uint32 loadFlags)
	: DisplayList(NULL)
{
	Font = font;
	Outset = outset;
	CharCode = ch;

	// Note: the default face is also used if no face (neither default nor fallback)
	// contains a glyph for the given char.
	Face = Font->Face;

	// search the FreeType char index (use default Unicode charmap) in the default face
	CharIndex = FT_Get_Char_Index(Face->Face, (FT_ULong) ch);
	if (CharIndex == 0)
	{
		// Glyph not in default font, search in fallback font faces
		for (FTFontFaceArray::iterator itr = Font->FallbackFaces.begin();
			itr != font->FallbackFaces.end(); ++itr)
		{
			CharIndex = FT_Get_Char_Index((*itr)->Face, (FT_ULong) ch);
			if (CharIndex != 0)
			{
				Face = (*itr);
				break;
			}
		}
	}

	Face->IncRef();
	CreateTexture(loadFlags);
}

void FTGlyph::CreateTexture(uint32 loadFlags)
{
	FT_Glyph glyph;
	FT_BitmapGlyph BitmapGlyph;
	FT_Bitmap * Bitmap;
	FT_BBox cbox;

	// We need vector data for outlined glyphs so do not load bitmaps.
	// This is necessary for mixed fonts that contain bitmap versions of smaller
	// glyphs, for example n CJK fonts.
	if (Outset > 0.0f)
		loadFlags |= FT_LOAD_NO_BITMAP;

	// Load the glyph for our character
	if (FT_Load_Glyph(Face->Face, CharIndex, loadFlags) != 0)
		throw FontException("FTGlyph::CreateTexture(): FT_Load_Glyph() failed for font '%s'.", Font->Filename.c_str());

	// Move the face's glyph into a FT_Glyph object.
	if (FT_Get_Glyph(Face->Face->glyph, &glyph) != 0)
		throw FontException("FTGlyph::CreateTexture(): FT_Get_Glyph() failed for font '%s'.", Font->Filename.c_str());

	if (Outset > 0.0f)
		StrokeBorder(glyph);

	// Store scaled advance width/height in glyph object
	Advance.X = Face->Face->glyph->advance.x / 64.0f + Outset*2;
	Advance.Y = Face->Face->glyph->advance.y / 64.0f + Outset*2;

	// Get the contour's bounding box (in 1/64th pixels, not font units)
	FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_UNSCALED, &cbox);

	// Convert 1/64th values to double values
	Bounds.Left = cbox.xMin / 64.0f;
	Bounds.Right = cbox.xMax / 64.0f + Outset*2;
	Bounds.Bottom = cbox.yMin / 64.0f;
	Bounds.Top = cbox.yMax / 64.0f + Outset*2;

	// Convert the glyph to a bitmap (and destroy original glyph image).
	// Request 8-bit greyscale pixel mode.
	FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, NULL, 1);
	BitmapGlyph = (FT_BitmapGlyph) glyph;

	// Get bitmap offsets
	BitmapCoords.Left = BitmapGlyph->left - cTexSmoothBorder;

	// Note: add 1*Outset for lifting the baseline so outset fonts do not intersect
	// with the baseline; ceil(Outset) for the outset pixels added to the bitmap.
	BitmapCoords.Top = BitmapGlyph->top + Outset + std::ceil(Outset) + cTexSmoothBorder;

	// Make accessing the bitmap easier
	Bitmap = &BitmapGlyph->bitmap;

	// Get bitmap dimensions
	BitmapCoords.Width = Bitmap->width + ((int) std::ceil(Outset) + cTexSmoothBorder) * 2;
	BitmapCoords.Height = Bitmap->rows + ((int) std::ceil(Outset) + cTexSmoothBorder) * 2;

	// Get power-of-2 bitmap widths
	TexSize.Width = NextPowerOf2(BitmapCoords.Width);
	TexSize.Height = NextPowerOf2(BitmapCoords.Height);

	// Texture widths ignoring empty (power of 2) padding space.
	TexOffset.X = (float) BitmapCoords.Width / (float) TexSize.Width;
	TexOffset.Y = (float) BitmapCoords.Height / (float) TexSize.Height;

	// Allocate memory for texture data
	uint8 * TexBuffer = new uint8[TexSize.Width * TexSize.Height];
	memset(TexBuffer, 0, TexSize.Width * TexSize.Height);
	uint8 * BitmapBuffer;

	// Freetype stores the bitmap with either upper (pitch is > 0) or lower
	// (pitch < 0) glyphs line first. Set the buffer to the upper line.
	// See http://freetype.sourceforge.net/freetype2/docs/glyphs/glyphs-7.html
	if (Bitmap->pitch > 0)
		BitmapBuffer = &Bitmap->buffer[0];
	else
		BitmapBuffer = &Bitmap->buffer[(Bitmap->rows - 1) * std::abs(Bitmap->pitch)];

	// copy data to texture bitmap (upper line first).
	for (int y = 0; y < Bitmap->rows; y++)
	{
		// set pointer to first pixel in line that holds bitmap data.
		// Each line starts with a cTexSmoothBorder pixel and multiple outset pixels
		// that are added by Extrude() later.
		uint8 * TexLine = TexBuffer + (y + cTexSmoothBorder + (int) std::ceil(Outset)) 
						* TexSize.Width + cTexSmoothBorder + (int) std::ceil(Outset);

		// get next lower line offset, use pitch instead of width as it tells
		// us the storage direction of the lines. In addition a line might be padded.
		uint8 * BitmapLine = &BitmapBuffer[y * Bitmap->pitch];

		// check for pixel mode and copy pixels
		// Should be 8 bit gray, but even with FT_RENDER_MODE_NORMAL, freetype
		// sometimes (e.g. 16px sized japanese fonts) fallbacks to 1 bit pixels.
		switch (Bitmap->pixel_mode)
		{
			// 8-bit gray
			case FT_PIXEL_MODE_GRAY:
			{
				for (int x = 0; x < Bitmap->width; x++)
					TexLine[x] = BitmapLine[x];
			} break;

			// 1-bit mono
			case FT_PIXEL_MODE_MONO:
			{
				for (int x = 0; x < Bitmap->width; x++)
					TexLine[x] = 255 * ((BitmapLine[x / 8] >> (7 - (x % 8))) & 1);
			} break;

			default:
				throw FontException("FTGlyph::CreateTexture(): Unhandled pixel format (%d).", Bitmap->pixel_mode);
		}
	}

	// allocate resources for textures and display lists
	glGenTextures(1, &Texture);

	// setup texture parameters
	glBindTexture(GL_TEXTURE_2D, Texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// create alpha-map (GL_ALPHA component only).
	// TexCoord (0,0) corresponds to the top left pixel of the glyph,
	// (1,1) to the bottom right pixel. So the glyph is flipped as OpenGL uses
	// a cartesian (y-axis up) coordinate system for textures.   
	// See the cTexSmoothBorder comment for info on texture borders.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, TexSize.Width, TexSize.Height,
		0, GL_ALPHA, GL_UNSIGNED_BYTE, &TexBuffer[0]);

	// free expanded data
	delete [] TexBuffer;

	// create the display list
	DisplayList = glGenLists(1);

	// render to display-list
	glNewList(DisplayList, GL_COMPILE);
	Render(false);
	glEndList();

	// free glyph data (bitmap, etc.)
	FT_Done_Glyph(glyph);
}

void FTGlyph::StrokeBorder(FT_Glyph Glyph)
{
	FT_Outline * Outline;
	FT_Stroker OuterStroker = NULL, InnerStroker = NULL;
	FT_UInt OuterNumPoints, InnerNumPoints, GlyphNumPoints;
	FT_UInt OuterNumContours, InnerNumContours, GlyphNumContours;
	FT_StrokerBorder OuterBorder, InnerBorder;
	FT_Int OutlineFlags;
	bool UseStencil;

	// It is possible to extrude the borders of a glyph with FT_Glyph_Stroke
	// but it will extrude the border to the outside and the inside of a glyph
	// although we just want to extrude to the outside.
	// FT_Glyph_StrokeBorder extrudes to the outside but also fills the interior
	// (this is what we need for bold fonts).
	// In both cases the inner font and outline font (border) will overlap.
	// Normally this does not matter but it does if alpha blending is active.
	// In this case if e.g. the inner color is set to white, the outline to red
	// and alpha to 0.5 the inner part will not be white it will be pink.

	// If we are to create the interior of an outlined font (fInner = true)
	// we have to create two borders:
	// - one extruded to the outside by fOutset pixels and
	// - one extruded to the inside by almost 0 zero pixels.
	// The second one is used as a stencil for the first one, clearing the
	// interiour of the glyph.
	// The stencil is not needed to create bold fonts.
	UseStencil = (Font->Part == fpInner);

	// we cannot extrude bitmaps, only vector based glyphs.
	// Check for FT_GLYPH_FORMAT_OUTLINE otherwise a cast to FT_OutlineGlyph is
	// invalid and FT_Stroker_ParseOutline() will crash
	if (Glyph->format != FT_GLYPH_FORMAT_OUTLINE)
		return;

	Outline = &((FT_OutlineGlyph) Glyph)->outline;
  
	OuterBorder = FT_Outline_GetOutsideBorder(Outline);
	if (OuterBorder == FT_STROKER_BORDER_LEFT)
		InnerBorder = FT_STROKER_BORDER_RIGHT;
	else
		InnerBorder = FT_STROKER_BORDER_LEFT;

	// extrude outer border
	if (FT_Stroker_New(Glyph->library, &OuterStroker) != 0)
		throw FontException("FTGlyph::StrokeBorder(): FT_Stroker_New() failed for font '%s'.", Font->Filename.c_str());

	FT_Stroker_Set(
		OuterStroker,
		(FT_Fixed) Round(Outset * 64),
		FT_STROKER_LINECAP_ROUND,
		FT_STROKER_LINEJOIN_BEVEL,
		(FT_Fixed) 0);

	// similar to FT_Glyph_StrokeBorder(inner = FT_FALSE) but it is possible to
	// use FT_Stroker_ExportBorder() afterwards to combine inner and outer borders
	if (FT_Stroker_ParseOutline(OuterStroker, Outline, 0) != 0)
		throw FontException("FTGlyph::StrokeBorder(): FT_Stroker_ParseOutline() failed for font '%s'.", Font->Filename.c_str());

	FT_Stroker_GetBorderCounts(OuterStroker, OuterBorder, &OuterNumPoints, &OuterNumContours);

	// extrude inner border (= stencil)
	if (UseStencil)
	{
		if (FT_Stroker_New(Glyph->library, &InnerStroker) != 0)
			throw FontException("FTGlyph::StrokeBorder(): FT_Stroker_New() failed for font '%s'.", Font->Filename.c_str());

		FT_Stroker_Set(
			InnerStroker,
			63, // extrude at most one pixel to avoid a black border
			FT_STROKER_LINECAP_ROUND,
			FT_STROKER_LINEJOIN_BEVEL,
			0);

		if (FT_Stroker_ParseOutline(InnerStroker, Outline, 0) != 0)
			throw FontException("FTGlyph::StrokeBorder(): FT_Stroker_ParseOutline() failed for font '%s'.", Font->Filename.c_str());

		FT_Stroker_GetBorderCounts(InnerStroker, InnerBorder, &InnerNumPoints, &InnerNumContours);
	}
	else
	{
		InnerNumPoints = 0;
		InnerNumContours = 0;
	}

	// combine borders (subtract: OuterBorder - InnerBorder)
	GlyphNumPoints = InnerNumPoints + OuterNumPoints;
	GlyphNumContours = InnerNumContours + OuterNumContours;

	// save flags before deletion (TODO: set them on the resulting outline)
	OutlineFlags = Outline->flags;

	// resize glyph outline to hold inner and outer border
	FT_Outline_Done(Glyph->library, Outline);
	if (FT_Outline_New(Glyph->library, GlyphNumPoints, GlyphNumContours, Outline) != 0)
		throw FontException("FTGlyph::StrokeBorder(): FT_Outline_New() failed for font '%s'.", Font->Filename.c_str());

	Outline->n_points = 0;
	Outline->n_contours = 0;

	// add points to outline. The inner-border is used as a stencil.
	FT_Stroker_ExportBorder(OuterStroker, OuterBorder, Outline);
	if (UseStencil)
		FT_Stroker_ExportBorder(InnerStroker, InnerBorder, Outline);

	if (FT_Outline_Check(Outline) != 0)
		throw FontException("FTGlyph::StrokeBorder(): FT_Stroker_ExportBorder() failed for font '%s'.", Font->Filename.c_str());

	if (InnerStroker != NULL)
		FT_Stroker_Done(InnerStroker);

	if (OuterStroker != NULL)
		FT_Stroker_Done(OuterStroker);
}

void FTGlyph::Render(bool useDisplayLists)
{
	// use display lists if enabled
	if (useDisplayLists)
	{
		glCallList(DisplayList);
		return;
	}

	glBindTexture(GL_TEXTURE_2D, Texture);
	glPushMatrix();

	// move to top left glyph position
	glTranslated(BitmapCoords.Left, BitmapCoords.Top, 0);

	// draw glyph texture
	glBegin(GL_QUADS);

	// top right
	glTexCoord2f(TexOffset.X, 0);
	glVertex2i(BitmapCoords.Width, 0);

	// top left
	glTexCoord2f(0, 0);
	glVertex2f(0, 0);

	// bottom left
	glTexCoord2f(0, TexOffset.Y);
	glVertex2i(0, -BitmapCoords.Height);

	// bottom right
	glTexCoord2f(TexOffset.X, TexOffset.Y);
	glVertex2i(BitmapCoords.Width, -BitmapCoords.Height);

	glEnd();

	glPopMatrix();
}

void FTGlyph::RenderReflection()
{
	const float CutOff = 0.6f;

	GLColor Color;
	float Descender;
	double UpperPos, TexUpperPos, TexLowerPos;

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, Texture);
	glGetFloatv(GL_CURRENT_COLOR, Color.vals);

	// add extra space to the left of the glyph
	glTranslated(BitmapCoords.Left, 0.0, 0.0);

	Descender = Font->GetDescender();

	// The upper position of the glyph, if CutOff is 1.0, it is fFont.Ascender.
	// If CutOff is set to 0.5 only half of the glyph height is displayed.
	UpperPos = Descender + Font->GetHeight() * CutOff;

	// the glyph texture's height is just the height of the glyph but not the font
	// height. Setting a color for the upper and lower bounds of the glyph results
	// in different color gradients. So we have to set the color values for the
	// descender and ascender (as we have a cutoff, for the upper-pos here) as
	// these positions are font but not glyph specific.

	// To get the texture positions we have to enhance the texture at the top and
	// bottom by the amount from the top to ascender (rather upper-pos here) and
	// from the bottom (Height-Top) to descender. Then we have to convert those
	// heights to texture coordinates by dividing by the bitmap Height and
	// removing the power-of-2 padding space by multiplying with fTexOffset.Y
	// (as fBitmapCoords.Height corresponds to fTexOffset.Y and not 1.0).
	TexUpperPos = -(UpperPos - BitmapCoords.Top) / BitmapCoords.Height * TexOffset.Y;
	TexLowerPos = (-(Font->GetDescender() + BitmapCoords.Height - BitmapCoords.Top) /
					BitmapCoords.Height + 1) * TexOffset.Y;

	// draw glyph texture
	glBegin(GL_QUADS);
	// top right
	glColor4f(Color.R, Color.G, Color.B, 0);
	glTexCoord2d(TexOffset.X, TexUpperPos);
	glVertex2d(BitmapCoords.Width, UpperPos);

	// top left
	glTexCoord2d(0.0, TexUpperPos);
	glVertex2d(0.0, UpperPos);

	// bottom left
	glColor4f(Color.R, Color.G, Color.B, Color.A - 0.3f);
	glTexCoord2d(0.0, TexLowerPos);
	glVertex2f(0.0f, Descender);

	// bottom right
	glTexCoord2d(TexOffset.X, TexLowerPos);
	glVertex2f((GLfloat) BitmapCoords.Width, Descender);
	glEnd();

	glPopMatrix();

	// restore old color
	// Note: glPopAttrib(GL_CURRENT_BIT)/glPopAttrib() is much slower then
	// glGetFloatv(GL_CURRENT_COLOR, ...)/glColor4fv(...)
	glColor4fv(Color.vals);
}

const FontPosition& FTGlyph::GetAdvance()
{
	return Advance;
}

const FontBounds& FTGlyph::GetBounds()
{
	return Bounds;
}

FTGlyph::~FTGlyph()
{
	Face->DecRef();
}

CachedFont::CachedFont(const path& filename)
	: FontBase(filename)
{
}

Glyph * CachedFont::GetGlyph(char ch)
{
	Glyph * glyph = Cache.GetGlyph(ch);
	if (glyph != NULL)
		return glyph;

	glyph = LoadGlyph(ch);
	if (Cache.AddGlyph(ch, glyph))
		return glyph;

	delete glyph;
	return NULL;
}

void CachedFont::FlushCache(bool keepBaseSet)
{
	Cache.FlushCache(keepBaseSet);
}

bool GlyphCache::AddGlyph(char ch, Glyph * glyph)
{
	uint32 baseCode = (ch >> 8);
	GlyphTable * glyphTable = FindGlyphTable(baseCode);
	if (glyphTable == NULL)
	{
		Hash[baseCode] = GlyphCacheHashEntry();
		glyphTable = &Hash[baseCode].Table;
	}

	// Get glyph table offset
	uint32 glyphCode = (ch & 0xff);

	// insert glyph into table if not present
	std::map<uint8, Glyph *>::iterator itr = glyphTable->find(glyphCode);
	if (itr == glyphTable->end())
	{
		glyphTable->insert(std::make_pair(glyphCode, glyph));
		return true;
	}

	return false;
}

Glyph * GlyphCache::GetGlyph(char ch)
{
	uint32 baseCode = (ch >> 8);
	GlyphTable * glyphTable = FindGlyphTable(baseCode);
	if (glyphTable == NULL)
		return NULL;

	// Get glyph table offset
	uint8 glyphCode = (ch & 0xff);

	// Lookup glyph from table
	std::map<uint8, Glyph *>::iterator itr = glyphTable->find(glyphCode);
	return (itr == glyphTable->end() ? NULL : itr->second);
}

GlyphTable * GlyphCache::FindGlyphTable(uint32 baseCode)
{
	std::map<uint32, GlyphCacheHashEntry>::iterator itr = Hash.find(baseCode);
	return (itr == Hash.end() ? NULL : &itr->second.Table);
}

void GlyphCache::FlushCache(bool keepBaseSet)
{
	// Destroy created glyphs
	if (!keepBaseSet)
	{
		// Clearing will free the GlyphCacheHashEntry objects which will free their Glyphs.
		Hash.clear();
		return;
	}

	for (std::map<uint32, GlyphCacheHashEntry>::iterator itr = Hash.begin();
		itr != Hash.end();)
	{
		std::map<uint32, GlyphCacheHashEntry>::iterator curItr = itr;
		++itr;

		GlyphCacheHashEntry& entry = curItr->second;

		// The base set (0-255) have a base code of 0 as the upper bytes are 0.
		if (curItr->first == 0)
			continue;

		// Removing will free the GlyphCacheHashEntry objects which will free their Glyphs.
		Hash.erase(curItr);
	}
}

GlyphCache::~GlyphCache()
{
	FlushCache(false);
}

FTFontFace::FTFontFace(const path& filename, int size)
	: Filename(filename), Size(size), RefCount(0)
{
}

void FTFontFace::Load()
{
	if (FT_New_Face(s_ftLibrary.GetLibrary(), Filename.generic_string().c_str(), 0, &Face) != 0)
		throw FontException("FTFontFace::Load(): Could not load font '%s'", Filename.c_str());

	// Support scalable fonts only
	if (!FT_IS_SCALABLE(Face))
		throw FontException("FTFontFace::Load(): Font '%s' is not scalable.", Filename.c_str());

	if (FT_Set_Pixel_Sizes(Face, 0, Size) != 0)
		throw FontException("FTFontFace::Load(): Could not set pixel size for '%s' to %d.", Filename.c_str(), Size);

	// Get scale factor for font unit to pixel size transformation
	FontUnitScale.X = (float) Face->size->metrics.x_ppem / (float) Face->units_per_EM;
	FontUnitScale.Y = (float) Face->size->metrics.y_ppem / (float) Face->units_per_EM;
}

FTFontFace::~FTFontFace()
{
	FT_Done_Face(Face);
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

void FTOutlineFont::DrawUnderline(const std::string& line)
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

void FTOutlineFont::Render(const std::string& line, bool reflectionPass)
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
	OutlineFont->Render(line, reflectionPass);
	glPopMatrix();

	// setup and render inner font
	glColor4fv(currentColor.vals);
	glTranslatef(Outset, Outset, 0.0f);
	glPushMatrix();
	InnerFont->Render(line, reflectionPass);
	glPopMatrix();
}

FontBounds FTOutlineFont::BBoxLines(const LineArray& lines, bool advance)
{
	return OutlineFont->BBoxLines(lines, advance);
}

void FTOutlineFont::SetOutlineColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a /*= -1.0f*/)
{
	OutlineColor.R = r;
	OutlineColor.G = g;
	OutlineColor.B = b;
	OutlineColor.A = a;
}

void FTOutlineFont::FlushCache(bool keepBaseSet)
{
	OutlineFont->FlushCache(keepBaseSet);
	InnerFont->FlushCache(keepBaseSet);
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
	for (int i = 0; i < SDL_arraysize(MipmapFonts); i++)
	{
		FontBase * font = MipmapFonts[i];
		if (font != NULL)
			static_cast<FTOutlineFont *>(font)->SetOutlineColor(r, g, b, a);
	}
}

void FTScalableOutlineFont::FlushCache(bool keepBaseSet)
{
	for (int i = 0; i < SDL_arraysize(MipmapFonts); i++)
	{
		FontBase * font = MipmapFonts[i];
		if (font != NULL)
			static_cast<FTOutlineFont *>(font)->FlushCache(keepBaseSet);
	}
}

float FTScalableOutlineFont::GetOutset()
{
	return static_cast<FTOutlineFont *>(BaseFont)->Outset * Scale;
}

FTFont::FTFont(const path& filename,
	int size, float outset /*= 0.0f*/, bool preCache /*= true*/,
	uint32 loadFlags /*= FT_LOAD_DEFAULT*/) : CachedFont(filename)
{
	Size = size;
	Outset = outset;
	PreCache = preCache;
	LoadFlags = loadFlags;
	UseDisplayLists = true;
	Part = fpNone;
	Face = GetFaceCache().LoadFace(filename, size);
	Face->IncRef();

	ResetIntern();

	// pre-cache some commonly used glyphs (' ' - '~')
	if (preCache)
	{
		for (char ch = ' '; ch < '~'; ch++)
		{
			FTGlyph * glyph = new FTGlyph(this, ch, outset, loadFlags);
			if (!Cache.AddGlyph(ch, glyph))
				delete glyph;
		}
	}
}

Glyph * FTFont::LoadGlyph(char ch)
{
	return new FTGlyph(this, ch, Outset, LoadFlags);
}

FontBounds FTFont::BBoxLines(const LineArray& lines, bool advance)
{
	FontBounds result = { 0.0f, 0.0f, 0.0f, 0.0f };
	FTGlyph * PrevGlyph = NULL;
	FT_Vector KernDelta;
	float UnderlinePos;

	// Reset global bounds
	result.Left = FLT_MAX;
	result.Bottom = FLT_MAX;

	// display text
	for (size_t LineIndex = 0; LineIndex < lines.size(); LineIndex++)
	{
		// get next text line
		const std::string& TextLine = lines[LineIndex];
		float LineYOffset = -LineSpacing * LineIndex;
		FontBounds LineBounds;

		// reset line bounds
		LineBounds.Left = LineBounds.Bottom = FLT_MAX;
		LineBounds.Right = LineBounds.Top = 0;

		// for each glyph image, compute its bounding box
		for (size_t CharIndex = 0; CharIndex < TextLine.size(); CharIndex++)
		{
			FTGlyph * Glyph = (FTGlyph *) GetGlyph(TextLine[CharIndex]);
			if (Glyph != NULL)
			{
				// get kerning
				if (UseKerning && FT_HAS_KERNING(Face->Face) && PrevGlyph != NULL)
				{
					FT_Get_Kerning(Face->Face, PrevGlyph->CharIndex, Glyph->CharIndex,
						FT_KERNING_UNSCALED, &KernDelta);
					LineBounds.Right = LineBounds.Right + KernDelta.x * Face->FontUnitScale.X;
				}

				// update left bound (must be done before right bound is updated)
				if (LineBounds.Right + Glyph->Bounds.Left < LineBounds.Left)
					LineBounds.Left = LineBounds.Right + Glyph->Bounds.Left;

				// update right bound
				if (CharIndex < (TextLine.size() - 1)	// not the last character
					|| (TextLine[CharIndex] == ' ')		// on space char (Bounds.Right = 0)
					|| advance)							// or in advance mode
				{
					// add advance && glyph spacing
					LineBounds.Right += Glyph->Advance.X + GlyphSpacing;
				}
				else
				{
					// add glyph's right bound
					LineBounds.Right += Glyph->Bounds.Right;
				}

				// update bottom && top bounds
				if (Glyph->Bounds.Bottom < LineBounds.Bottom)
					LineBounds.Bottom = Glyph->Bounds.Bottom;

				if (Glyph->Bounds.Top > LineBounds.Top)
					LineBounds.Top = Glyph->Bounds.Top;
			}

			PrevGlyph = Glyph;
		}

		// handle italic font style
		if (Style & fsItalic)
		{
			LineBounds.Left += LineBounds.Bottom * cShearFactor;
			LineBounds.Right += LineBounds.Top * cShearFactor;
		}

		// handle underlined font style
		if (Style & fsUnderline)
		{
			UnderlinePos = GetUnderlinePosition();
			if (UnderlinePos < LineBounds.Bottom)
				LineBounds.Bottom = UnderlinePos;
		}

		// add line offset
		LineBounds.Bottom = LineBounds.Bottom + LineYOffset;
		LineBounds.Top = LineBounds.Top + LineYOffset;

		// adjust global bounds
		if (result.Left > LineBounds.Left)
			result.Left = LineBounds.Left;
		if (result.Right < LineBounds.Right)
			result.Right = LineBounds.Right;
		if (result.Bottom > LineBounds.Bottom)
			result.Bottom = LineBounds.Bottom;
		if (result.Top < LineBounds.Top)
			result.Top = LineBounds.Top;
  }

	// if left or bottom bound was not set, set them to 0
	if (result.Left == FLT_MAX)
		result.Left = 0.0f;
	if (result.Bottom == FLT_MAX)
		result.Bottom = 0.0f;

	return result;
}

void FTFont::AddFallback(const path& filename)
{
	FTFontFace * fontFace = GetFaceCache().LoadFace(filename, Size);
	FallbackFaces.push_back(fontFace);
}

float FTFont::GetUnderlinePosition()
{
	return Face->Face->underline_position * Face->FontUnitScale.Y - Outset;
}

float FTFont::GetUnderlineThickness()
{
	return Face->Face->underline_thickness * Face->FontUnitScale.Y - Outset*2;
}

float FTFont::GetHeight()
{
	return GetAscender() - GetDescender();
}

float FTFont::GetAscender()
{
	return Face->Face->ascender * Face->FontUnitScale.Y + Outset*2;
}

float FTFont::GetDescender()
{
	// Note: outset is not part of the descender as the baseline is lifted
	return Face->Face->descender * Face->FontUnitScale.Y;
}

void FTFont::Render(const std::string& text, bool reflectionPass /*= false*/)
{
	FTGlyph * PrevGlyph = NULL;

	// draw current line
	for (size_t CharIndex = 0; CharIndex < text.size(); CharIndex++)
	{
		FTGlyph * Glyph = (FTGlyph *) GetGlyph(text[CharIndex]);
		if (Glyph != NULL)
		{
			// get kerning
			if (UseKerning
				&& FT_HAS_KERNING(Face->Face)
				&& PrevGlyph != NULL)
			{
				FT_Vector KernDelta;
				FT_Get_Kerning(Face->Face, PrevGlyph->CharIndex, Glyph->CharIndex,
							   FT_KERNING_UNSCALED, &KernDelta);
				glTranslatef(KernDelta.x * Face->FontUnitScale.X, 0, 0);
			}

			if (reflectionPass)
				Glyph->RenderReflection();
			else
				Glyph->Render(UseDisplayLists);

			glTranslatef(Glyph->Advance.X + GlyphSpacing, 0.0f, 0.0f);
		}

		PrevGlyph = Glyph;
	}
}

FTFont::~FTFont()
{
	GetFaceCache().UnloadFace(Face);
	for (FTFontFaceArray::iterator itr = FallbackFaces.begin(); itr != FallbackFaces.end(); ++itr)
		GetFaceCache().UnloadFace(*itr);
	FallbackFaces.clear();

	if (Face != NULL)
		Face->DecRef();
}

FreeType::FreeType()
{
	if (FT_Init_FreeType(&_ftLibrary) != 0)
		throw FontException("FT_Init_FreeType() failed");
}

FT_Library& FreeType::GetLibrary()
{
	return _ftLibrary;
}

FreeType::~FreeType()
{
	FT_Done_FreeType(_ftLibrary);
}
