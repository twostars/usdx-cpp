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

#ifndef _TEXTUREMGR_H
#define _TEXTUREMGR_H
#pragma once

#include "Texture.h"

struct TextureEntry
{
	path	Name;
	eTextureType			Type;
	Uint32					Color;
	Texture					Tex;		// Full-size texture
	Texture					TexCache;	// Thumbnail texture
};

class TextureMgr : public Singleton<TextureMgr>
{
public:
	// FIXME: We should be able to index this properly.
	// Searches are based on filename, texture type & color.
	// Regular lookups use integral IDs, but not sure how much 
	// actually refers to them this way currently.
	typedef std::vector<TextureEntry> TextureDatabase;

	TextureMgr();

	Texture CreateTexture(const Uint8* data, const path* texturePath,
		Uint16 width, Uint16 height);
	void AddTexture(const Texture& tex, eTextureType textureType, 
		Uint32 color = 0, bool cache = false);
	Texture GetTexture(const path* texturePath, 
		eTextureType textureType, Uint32 color = 0, bool fromCache = false);
	Texture LoadTexture(const path* texturePath, 
		eTextureType textureType = TextureType::Plain, Uint32 color = 0);
	int FindTexture(const Texture& tex, eTextureType textureType, Uint32 color);
	void UnloadTexture(const path* texturePath, eTextureType textureType, 
		Uint32 color = 0, bool fromCache = false);

	~TextureMgr();

	TextureDatabase Textures;
	int Limit;
};

#define sTextureMgr (TextureMgr::getSingleton())

#endif
