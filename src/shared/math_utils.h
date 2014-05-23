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

#ifndef _MATH_UTILS_H
#define _MATH_UTILS_H
#pragma once

// Squares the specified number. 
template <typename T>
INLINE T sqr(T val) { return val * val; }

// Calculates log2 of number.  
INLINE double Log2(double n) { return std::log(n) / std::log(2.0); }

// Rounds the specified number.
template <typename T>
INLINE T Round(T val) { return static_cast<T>(std::ceil(val - 0.5f)); }

#endif
