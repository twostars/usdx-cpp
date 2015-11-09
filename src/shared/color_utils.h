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

#ifndef _COLOR_UTILS_H
#define _COLOR_UTILS_H
#pragma once

RGB GetSystemColor(eColor color)
{
	RGB v;

	switch (color)
	{
	case Color::Blue:
		v.Set(71, 175, 247);
		break;

	case Color::Green:
		v.Set(63, 191, 63);
		break;

	case Color::Pink:
		v.Set(255, 175, 247);
		break;

	case Color::Red:
		v.Set(247, 71, 71);
		break;

	case Color::Violet:
		v.Set(212, 71, 247);
		break;

	case Color::Orange:
		v.Set(247, 144, 71);
		break;

	case Color::Yellow:
		v.Set(230, 230, 95);
		break;

	case Color::Brown:
		v.Set(192, 127, 31);
		break;

	case Color::Black:
	default:
		v.Set(0, 0, 0);
	}

	return v;
}

// Load RGB in "n n n" format
// e.g. 255 255 255
void RGB::Load(const char * source)
{
	uint32 r, g, b;
	sscanf(source, "%u %u %u", &r, &g, &b);
	Set((uint8) r, (uint8) g, (uint8) b);
}

void RGB::Set(uint8 r, uint8 g, uint8 b)
{
	R = Transform(r);
	G = Transform(g);
	B = Transform(b);
}

float RGB::Transform(uint8 val)
{
	return (float) val / 255.0f;
}

void RGB::Sqrt(const RGB& rhs)
{
	R = sqrt(rhs.R);
	G = sqrt(rhs.G);
	B = sqrt(rhs.B);
}

uint32 RGB::ToUInt32() const
{
	uint32 r = (uint8)(255 * R),
		   g = (uint8)(255 * G),
		   b = (uint8)(255 * B);

	return (uint32)(r << 16) | (uint32)(g << 8) | (uint32)(b);
}

#endif
