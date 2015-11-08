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

#ifndef _UNICODE_H
#define _UNICODE_H
#pragma once

#if !defined(WIN32)
#define _snprintf	snprintf
#endif

#define __T(id) sLanguage.Translate(id)

#if defined(_MSC_VER) && _MSC_VER < 1900
#define snprintf(buf, len, format, ...) _snprintf_s(buf, len,len, format, __VA_ARGS__)
#endif

#define _STR(str) #str
#define STRINGIFY(str) _STR(str)

#endif
