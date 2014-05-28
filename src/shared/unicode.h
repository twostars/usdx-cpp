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
#	define _tcschr		strchr
#	define _tcscmp		strcmp
#	define TCHAR		char

#endif

#define TSTR(str) _T(#str)
#define TSTRINGIFY(str) TSTR(str)

// NOTE: Use the preprocessor instead of typedefing to avoid strict type-checks
#ifdef UNICODE
#	define tstring		std::wstring
#	define tstring_ci	wstring_ci
#	define TSTRCASECMP	WSTRCASECMP
#	define StrSplit		StrSplitW
#	define strpos		strposW
#else
#	define tstring		std::string
#	define tstring_ci	string_ci
#	define TSTRCASECMP	STRCASECMP
#	define StrSplit		StrSplitA
#	define strpos		strposA
#endif

typedef tstring::size_type tsize_type;

#endif
