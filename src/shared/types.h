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

#ifndef _TYPES_H
#define _TYPES_H
#pragma once

#include <cstdint>

#ifdef _WIN32
#	define INLINE __forceinline
#else
#	define INLINE inline
#endif

#if defined(RGB)
#	undef RGB
#endif

struct RGB
{
	float R, G, B;

	void Load(const char * source);
	void Sqrt(const RGB& rhs);

	void Set(Uint8 r, Uint8 g, Uint8 b);
	static float Transform(Uint8 val);

	Uint32 ToUInt32() const;
};

struct RGBA
{
	float R, G, B, A;
};

#endif
