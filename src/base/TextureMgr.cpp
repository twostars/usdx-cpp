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
#include "Log.h"
#include "TextureMgr.h"
#include "Graphic.h"

initialiseSingleton(TextureMgr);

TextureMgr::TextureMgr()
{
}

Texture TextureMgr::CreateTexture(
	const uint8* data, 
	const boost::filesystem::path* texturePath,
	uint16 width, uint16 height)
{
	Texture tex;
	GLuint ActTex;

	if (texturePath == NULL
		|| texturePath->empty())
	{
		assert(0);
		return tex;
	}

	glGenTextures(1, &ActTex);
	glBindTexture(GL_TEXTURE_2D, ActTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

#if defined(BIG_ENDIAN)
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
#else
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
#endif

	tex.TexNum = ActTex;
	tex.Name = texturePath->native();

	return tex;
}

void TextureMgr::AddTexture(
	const Texture& tex, eTextureType textureType, 
	uint32 color /*= 0*/, bool cache /*= false*/)
{
	int textureIndex = FindTexture(tex, textureType, color);
	if (textureIndex < 0)
	{
		TextureEntry entry;

		entry.Name = tex.Name;
		entry.Type = textureType;
		entry.Color = color;

		textureIndex = (int) Textures.size();
		Textures.push_back(entry);
	}

	if (cache)
		Textures[textureIndex].TexCache = tex;
	else
		Textures[textureIndex].Tex = tex;
}

Texture TextureMgr::GetTexture(
	const boost::filesystem::path* texturePath, 
	eTextureType textureType, uint32 color /*= 0*/, 
	bool fromCache /*= false*/)
{
	Texture tex;
	int textureIndex;

	if (texturePath == NULL
		|| texturePath->empty())
	{
		assert(0);
		return tex;
	}

	tex.Name = texturePath->native(); /* hack */
	textureIndex = FindTexture(tex, textureType, color);

	/* Pull thumbnail/cache texture */
	if (fromCache)
	{
		if (textureIndex >= 0)
			return Textures[textureIndex].TexCache;

		return tex; // hm.
	}

	/* Pull full texture */

	// Not found in cache, so add it.
	if (textureIndex < 0)
	{
		textureIndex = Textures.size();
		AddTexture(tex, textureType, color, fromCache);
	}

	// Load the full texture
	if (Textures[textureIndex].Tex.TexNum == 0)
		Textures[textureIndex].Tex = LoadTexture(texturePath, textureType, color);

	return Textures[textureIndex].Tex;
}

Texture TextureMgr::LoadTexture(
	const boost::filesystem::path* texturePath, 
	eTextureType textureType /*= TextureType::Plain*/, uint32 color /*= 0*/)
{
	Texture tex;
	if (texturePath == NULL
		|| texturePath->empty())
		return tex;

	SDL_Surface * texSurface = LoadSurfaceFromFile(*texturePath);
	if (texSurface == NULL)
	{
		sLog.Error(_T("TextureMgr::LoadTexture"), _T("Could not load texture '%s' with type '%s'"),
			texturePath->native().c_str(), Enum2String(textureType));
		return tex;
	}

	// Convert pixel format as needed
	AdjustPixelFormat(texSurface, textureType);

	// Adjust texture size (scale down, if necessary)
	int newWidth = texSurface->w;
	int newHeight = texSurface->h;

	if (newWidth > Limit)
		newWidth = Limit;

	if (newHeight > Limit)
		newHeight = Limit;

	if (texSurface->w > newWidth || texSurface->h > newHeight)
		ScaleImage(texSurface, newWidth, newHeight);

	// Now we might colorize the whole thing
	if (textureType == TextureType::Colorized)
		ColorizeImage(texSurface, color);

	// Save actual dimensions of our texture
	int oldWidth = newWidth;
	int oldHeight = newHeight;

	// Make texture dimensions be powers of 2
	newWidth = (int) std::pow(2, std::ceil(Log2(newWidth)));
	newHeight = (int) std::pow(2, std::ceil(Log2(newHeight)));

	if (newWidth != oldWidth || newHeight != oldHeight)
		FitImage(texSurface, newWidth, newHeight);

	// At this point we have the image in memory
	// scaled so that dimensions are powers of 2
	// and converted to either RGB or RGBA.
	// If we have a texture of type Plain, Transparent or Colorized,
	// then we're done manipulating it and can now create our OpenGL 
	// texture from it.

	// Prepare OpenGL texture
	GLuint ActTex;
	glGenTextures(1, &ActTex);

	glBindTexture(GL_TEXTURE_2D, ActTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Load data into OpenGL texture
	if (textureType == TextureType::Transparent
		|| textureType == TextureType::Colorized)
	{
#if defined(BIG_ENDIAN)
		glTexImage2D(GL_TEXTURE_2D, 0, 4, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, texSurface->pixels);
#else
		glTexImage2D(GL_TEXTURE_2D, 0, 4, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texSurface->pixels);
#endif
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, 4, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texSurface->pixels);
	}

	// Setup texture
	tex.W = (float) oldWidth;
	tex.H = (float) oldHeight;
	tex.TexNum = ActTex;
	tex.TexW = (float)(oldWidth / newWidth);
	tex.TexH = (float)(oldHeight / newHeight);
	tex.Name = texturePath->native();

	UnloadSurface(texSurface);
	return tex;
}

int TextureMgr::FindTexture(
	const Texture& tex, eTextureType textureType, uint32 color)
{
	int index = 0;
	for (TextureDatabase::iterator itr = Textures.begin(); itr != Textures.end(); ++itr, ++index)
	{
		TextureEntry& currentTex = (*itr);
		if (currentTex.Type == textureType
			&& !currentTex.Name.empty()
			&& currentTex.Name.native() == tex.Name
			// Colorized textures must match in their color
			&& (currentTex.Type != TextureType::Colorized || currentTex.Color == color))
			return index;
	}

	return -1;
}

void TextureMgr::UnloadTexture(
	const boost::filesystem::path* texturePath, eTextureType textureType, 
	uint32 color /*= 0*/, bool fromCache /*= false*/)
{
	Texture lookupTex;
	lookupTex.Name = texturePath->native(); // hack
	int textureNo = FindTexture(lookupTex, textureType, color);
	if (textureNo <= 0)
		return;

	TextureEntry& entry = Textures[textureNo];
	Texture& tex = (fromCache ? entry.TexCache : entry.Tex);
	glDeleteTextures(1, (const GLuint *)&tex.TexNum);
	tex.TexNum = 0;
}

TextureMgr::~TextureMgr()
{
	for (TextureDatabase::iterator itr = Textures.begin(); itr != Textures.end(); ++itr)
	{
		if ((*itr).Tex.TexNum > 0)
		{
			glDeleteTextures(1, (const GLuint *)&(*itr).Tex.TexNum);
			(*itr).Tex.TexNum = 0;
		}

		if ((*itr).TexCache.TexNum > 0)
		{
			glDeleteTextures(1, (const GLuint *)&(*itr).TexCache.TexNum);
			(*itr).TexCache.TexNum = 0;
		}
	}

	Textures.clear();
}
