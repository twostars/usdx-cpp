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

#ifndef _TEXTURE_H
#define _TEXTURE_H
#pragma once

struct Texture
{
	uint32 TexNum;
	float  X, Y, Z;
	float  W, H;
	float  ScaleW; // for dynamic scaling while leaving width constant
	float  ScaleH; // for dynamic scaling while leaving height constant
	float  Rot;    // 0 - 2*pi
	float  Int;    // intensity
	RGB    ColRGB;
	float  TexW;   // percentage of width to use [0..1]
	float  TexH;   // percentage of height to use [0..1]
	float  TexX1, TexY1;
	float  TexX2, TexY2;
	float  Alpha;
	tstring Name; // experimental for handling cache images. maybe it's useful for dynamic skins
};

enum TextureType
{
	TEXTURE_TYPE_PLAIN,        // Plain (alpha = 1)
	TEXTURE_TYPE_TRANSPARENT,  // Alpha is used
	TEXTURE_TYPE_COLORIZED     // Alpha is used; Hue of the HSV color-model will be replaced by a new value
};

#endif