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

#ifndef _STRING_UTILS_H
#define _STRING_UTILS_H
#pragma once

#include <algorithm>

#if defined(WIN32)
#	define STRCASECMP	_stricmp
#	define WSTRCASECMP	_wcsicmp
#else
#	define STRCASECMP strcasecmp
#	define WSTRCASECMP	wcscasecmp
#endif

struct string_ci
{
	bool operator() (const std::string& str1, const std::string& str2) const {
		return STRCASECMP(str1.c_str(), str2.c_str()) < 0;
	}
};

struct wstring_ci
{
	bool operator() (const std::wstring& str1, const std::wstring& str2) const {
		return WSTRCASECMP(str1.c_str(), str2.c_str()) < 0;
	}
};

INLINE void strtolower(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), tolower);
}

INLINE void strtolower(std::wstring& str)
{
	std::transform(str.begin(), str.end(), str.begin(), towlower);
}

INLINE void strtoupper(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), toupper);
}

INLINE void strtoupper(std::wstring& str)
{
	std::transform(str.begin(), str.end(), str.begin(), towupper);
}

#endif