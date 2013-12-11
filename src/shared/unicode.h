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

// Windows has the tchar header, with various per-build typedefs.
#if defined(_WIN32)

#	include <tchar.h>

// Use the preprocessor instead of typedefing to avoid strict type-checks
#	ifdef _UNICODE
#		define tstring		std::wstring
#	else
		define tstring		std::string
#	endif

// Other operating systems don't have the header, so define everything for ANSI.
#else 

#	define TCHAR		char
#	define _T(x)		x
#	define tstring		std::string
#	define _tprintf		printf
#	define _stscanf		sscanf
#	define _ttoi		atoi
#	define _tfopen		fopen
#	define _ftprintf	fprintf
#	define _snprintf	snprintf
#	define _sntprintf	_snprintf
#	define _vsntprintf  vsnprintf

#endif

#define TSTR(str) _T(#str)
#define TSTRINGIFY(str) TSTR(str)

#ifdef UNICODE
#	define tstring_ci	wstring_ci
#else
#	define tstring_ci	string_ci
#endif

#endif
