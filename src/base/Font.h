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

#ifndef _FONT_H
#define _FONT_H
#pragma once

#include <SDL_ttf.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_STROKER_H

// Enables the Freetype font cache
#define ENABLE_FT_FACE_CACHE 1

// Flip direction of y-axis.
// Default is a cartesian coordinate system with y-axis in upper direction
// but with USDX the y-axis is in lower direction.
#define FLIP_YAXIS 1
#define BITMAP_FONT 1

enum FontStyle
{
	fsItalic	= (1 << 0),
	fsUnderline	= (1 << 1),
	fsReflect	= (1 << 2)
};

enum FontPart
{
	fpNone,
	fpInner,
	fpOutline
};

struct FontBounds
{
	float Left, Right;
	float Bottom, Top;
};

struct FontPosition
{
	float X, Y;
};

struct TextureSize
{
	int Width, Height;
};

struct BitmapCoords
{
	double Left, Top;
	int Width, Height;
};

#pragma pack(push, 1)
struct GLColor
{
	union
	{
		float R, G, B, A;
		float vals[4];
	};
};
#pragma pack(pop)

DECLARE_EXCEPTION(FontException, BaseException);

class FontBase
{
public:
	typedef std::vector<tstring> LineArray;

	FontBase();
	FontBase(const path& filename);

	void SetFilename(const path& filename);
	void ResetIntern();

	virtual void Init() = 0;

	virtual void SplitLines(const tstring& string, LineArray& lines);
	virtual void PrintLines(const LineArray& lines, bool reflectionPass = false);
	virtual void Print(const tstring& text);
	virtual void DrawUnderline(const tstring& line);
	virtual void Render(const tstring& line) = 0;
	virtual FontBounds BBox(const tstring& text, bool advance = true);
	virtual FontBounds BBoxLines(const LineArray& lines, bool advance) = 0;

	// Adds a new font that is used if the default font misses a glyph
	// Throws FontException if the fallback could not be initialized.
	virtual void AddFallback(const path& filename) = 0;

	virtual float GetUnderlinePosition() = 0;
	virtual float GetUnderlineThickness() = 0;

	virtual float GetHeight() = 0;
	virtual float GetAscender() = 0;
	virtual float GetDescender() = 0;
    
	virtual ~FontBase();

	path Filename;
	uint8 Style; // FontStyle
	bool UseKerning;
	float LineSpacing; // must be set by subclass
	float ReflectionSpacing; // must be set by subclass to -2*Descender
	float GlyphSpacing;
};

// Abstract base class representing a glyph
class Glyph
{
public:
	virtual void Render(bool useDisplayLists) = 0;
	virtual void RenderReflection() = 0;

	// Distance to next glyph (in pixels)
	virtual const FontPosition& GetAdvance() = 0;

	// Glyph bounding box (in pixels)
	virtual const FontBounds& GetBounds() = 0;
};

class GlyphCacheHashEntry
{
public:
	~GlyphCacheHashEntry();
	std::map<uint8, Glyph *> GlyphTable;
};

class GlyphCache
{
public:
	/**
	* Adds glyph with char-code ch to the cache.
	* @returns true on success, false otherwise
	*/
	bool AddGlyph(TCHAR ch, const Glyph * glyph);
	void FlushCache(bool keepBaseSet);
	~GlyphCache();

	std::map<uint8, GlyphCacheHashEntry> Hash;
};

// FreeType font face class.
class FTFontFace
{
public:
	FTFontFace(const path& filename, int size);
	void Load();
	~FTFontFace();

	path Filename;	//**< filename of the font-file
	FT_Face Face;					//**< Holds the height of the font
	FontPosition FontUnitScale;			//**< FT font-units to pixel ratio
	int Size;

	int RefCount; // TODO: Move this into its own class & make it atomic. Not a priority as it's only used in a single-thread anyway.
	void IncRef() { ++RefCount; }
	int DecRef() 
	{
		int refCount = --RefCount;
		if (refCount == 0) 
			delete this; 
		return refCount;
	}
};

typedef std::vector<FTFontFace *> FTFontFaceArray;

/**
* Loading font faces with freetype is a slow process.
* Especially loading a font (e.g. fallback fonts) more than once is a waste
* of time. Just cache already loaded faces here.
*/
class FTFontFaceCache
{
public:
	/**
	* @raises FontException if the font could not be initialized
	*/
	FTFontFace * LoadFace(const path& filename, int size);
	void UnloadFace(FTFontFace * face);

	std::set<FTFontFace *> Faces;
};

class FTFont;
class FTGlyph : public Glyph
{
public:
	/**
	* Creates a glyph with char-code ch from font Font.
	* @param loadFlags flags passed to FT_Load_Glyph()
	*/
	FTGlyph(FTFont * font, TCHAR ch, float outset, uint32 loadFlags);

	/**
	* Creates an OpenGL texture (and display list) for the glyph.
	* The glyph's and bitmap's metrics are set correspondingly.
	* @param  LoadFlags  flags passed to FT_Load_Glyph()
	* @raises FontException  if the glyph could not be initialized
	*/
	void CreateTexture(uint32 loadFlags);

	/**
	* Extrudes the outline of a glyph's bitmap stored in TexBuffer with size
	* fTexSize by Outset pixels.
	* This is useful to create bold or outlined fonts.
	* TexBuffer must be 2*Ceil(Outset) pixels higher and wider than the
	* original glyph bitmap, otherwise the glyph borders cannot be extruded
	* correctly.
	* The bitmap must be 2* pixels wider and higher than the
	* original glyph's bitmap with the latter centered in it.
	*/
	void StrokeBorder(FT_Glyph glyph);

	// Renders the glyph (normal render pass)
	virtual void Render(bool useDisplayLists);

	// Renders the glyph's reflection
	virtual void RenderReflection();

	// Distance to next glyph (in pixels)
	virtual const FontPosition& GetAdvance();

	// Glyph bounding box (in pixels)
	virtual const FontBounds& GetBounds();

	~FTGlyph();

protected:
	TCHAR CharCode;				//**< Char code
	FTFontFace * Face;			//**< Freetype face used for this glyph
	FT_UInt CharIndex;			//**< Freetype specific char-index (<> char-code)
	GLuint DisplayList;			//**< Display-list ID
	GLuint Texture;				//**< Texture ID
	BitmapCoords BitmapCoords;	//**< Left/Top offset and Width/Height of the bitmap (in pixels)
	FontPosition TexOffset;		//**< Right and bottom texture offset for removal of power-of-2 padding
	TextureSize TexSize;		//**< Texture size in pixels

	FTFont * Font;				//**< Font associated with this glyph
	FontPosition Advance;		//**< Advance width of this glyph
	FontBounds Bounds;			//**< Glyph bounds
	float Outset;				//**< Extrusion outset
};

class CachedFont : public FontBase
{
public:
	CachedFont(const path& filename);
	virtual void Init() {}
	void FlushCache(bool keepBaseSet);

protected:
	GlyphCache Cache;
};

class ScalableFont : public FontBase
{
public:
	static const int MaxMipmapLevel = 5;

	ScalableFont(bool useMipMaps);

	void SetBaseFont(FontBase * baseFont);
	void ResetIntern();

	virtual void Init();
	virtual FontBase * CreateMipmap(int level, float scale) = 0;
	virtual FontBounds BBoxLines(const LineArray& lines, bool advance);

	/**
	* Chooses the mipmap that looks nicest with current scale and projection
	* matrix.
	*/
	FontBase * ChooseMipmapFont();

	/**
	* Returns the mipmap level considering the current scale and projection
	* matrix.
	*/
	int GetMipmapLevel();

	virtual void PrintLines(const LineArray& lines, bool reflectionPass = false);
	virtual void Render(const tstring& text);

	virtual float GetUnderlinePosition();
	virtual float GetUnderlineThickness();

	virtual void SetHeight(float height);
	virtual float GetHeight();
	virtual float GetAscender();
	virtual float GetDescender();

	~ScalableFont();

	float Scale;
	float Stretch;
	FontBase * BaseFont;
	bool UseMipmaps;

	// Mipmap fonts (size[level+1] = size[level]/2)
	FontBase * MipmapFonts[MaxMipmapLevel + 1];
};

// FreeType font class
class FTFont : public CachedFont
{
public:
	/**
	* Creates a font of size Size (in pixels) from the file Filename.
	* If Outset (in pixels) is set to a value > 0 the glyphs will be extruded
	* at their borders. Use it for e.g. a bold effect.
	* @param  LoadFlags  flags passed to FT_Load_Glyph()
	* @raises EFontError  if the font-file could not be loaded
	*/
	FTFont(const path& filename,
		int size, float outset = 0.0f, bool precache = true,
		uint32 loadFlags = 0);

	static FTFontFaceCache& GetFaceCache() { return s_fontFaceCache; }

	virtual FontBounds BBoxLines(const LineArray& lines, bool advance);
	virtual void AddFallback(const path& filename);

	virtual float GetUnderlinePosition();
	virtual float GetUnderlineThickness();
	virtual float GetHeight();
	virtual float GetAscender();
	virtual float GetDescender();

	virtual void Render(const tstring& text);

	~FTFont();

	FTFontFace * Face;				//**< Default font face
	int Size;						//**< Font base size (in pixels)
	float Outset;					//**< size of outset extrusion (in pixels)
	bool PreCache;					//**< pre-load base glyphs
	uint32 LoadFlags;				//**< FT glpyh load-flags
	bool UseDisplayLists;			//**< true: use display-lists, false: direct drawing
	FontPart Part;						//**< indicates the part of an outline font
	FTFontFaceArray FallbackFaces;	//**< available fallback faces, ordered by priority

	static FTFontFaceCache s_fontFaceCache;
};

class FTScalableFont : public ScalableFont
{
public:
	/**
	* Creates a scalable font of size Size (in pixels) from the file Filename.
	* OutsetAmount is the ratio of the glyph extrusion.
	* The extrusion in pixels is Size*OutsetAmount
	* (0.0 -> no extrusion, 0.1 -> 10%).
	*
	* The memory size (in bytes) consumed by a scalable font
	* - with UseMipmaps=false:
	*  mem = size^2 * #cached_glyphs
	* - with UseMipmaps=true (all mipmap levels):
	*  mem = size^2 * #cached_glyphs * Sum[i=1..cMaxMipmapLevel](1/i^2)
	* - with UseMipmaps=true (5 <= cMaxMipmapLevel <= 10):
	*  mem ~= size^2 * #cached_glyphs * 1.5
	*
	* Examples (for 128 cached glyphs):
	* - Size: 64 pixels: 768 KB (mipmapped) or 512 KB (non-mipmapped).
	* - Size 128 pixels: 3 MB (mipmapped) or 2 MB (non-mipmapped)
	*
	* Note: once a glyph is cached there will.
	*/
	FTScalableFont(const path& filename,
			int size, float outsetAmount = 0.0f, bool useMipmaps = true,
			bool preCache = true);

	virtual void AddFallback(const path& filename);

protected:
	virtual FontBase * CreateMipmap(int level, float scale);
};

/**
* Represents a FreeType font with an additional outline around its glyphs.
* The outline size is passed on creation and cannot be changed later.
*/
class FTOutlineFont : public FontBase
{
public:
	FTOutlineFont(const path& filename, int size, float outset,
		bool preCache = true, uint32 loadFlags = FT_LOAD_DEFAULT);

	virtual void Init() {}
	void ResetIntern();
	virtual void AddFallback(const path& filename);
	virtual void DrawUnderline(const tstring& line);
	virtual void Render(const tstring& line);
	virtual FontBounds BBoxLines(const LineArray& lines, bool advance);

	/**
	* Sets the color of the outline.
	* If the alpha component is < 0, OpenGL's current alpha value will be
	* used.
	*/
    void SetOutlineColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a = -1.0f);

    /** @seealso GlyphCache.FlushCache */
    void FlushCache(bool keepBaseSet);

	virtual float GetUnderlinePosition();
	virtual float GetUnderlineThickness();
	virtual float GetHeight();
	virtual float GetAscender();
	virtual float GetDescender();

	~FTOutlineFont();

	int Size;
	float Outset;
	FTFont * InnerFont;
	FTFont * OutlineFont;
	GLColor OutlineColor;
	bool PreCache;
};

/**
* Wrapper around OutlineFont to allow font resizing.
* @seealso ScalableFont
*/
class FTScalableOutlineFont : public ScalableFont
{
public:
	FTScalableOutlineFont(const path& filename, int size, 
		float outsetAmount, bool useMipmaps = true, bool preCache = true);

	virtual void AddFallback(const path& filename);

	void SetOutlineColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a = -1.0f);
	void FlushCache(bool keepBaseSet);

	virtual float GetOutset();

protected:
	virtual FontBase * CreateMipmap(int level, float scale);
};

class GLFont
{
public:
	GLFont()
	{
		Font = NULL;
		Outlined = false;
		X = Y = Z = 0.0f;
	}

	~GLFont()
	{
		delete Font;
	}

	ScalableFont * Font;
	bool Outlined;
	float X, Y, Z;
};

class FreeType
{
public:
	FreeType();
	FT_Library& GetLibrary();
	~FreeType();

protected:
	FT_Library _ftLibrary;
};

#endif
