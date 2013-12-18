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

#if defined(WIN32)
#	define STRCASECMP	_stricmp
#	define WSTRCASECMP	_wcsicmp
#else
#	define STRCASECMP	strcasecmp
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

template<int (&F)(int)> int safe_ctype(unsigned char c) { return F(c); } 
static int safe_isspace(int c) { return safe_ctype<std::isspace>(c); }

// trim from end
template <typename STRING_TYPE>
STRING_TYPE& rtrim(STRING_TYPE &s) 
{
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(safe_isspace))).base(), s.end());
	return s;
}

// trim from start
template <typename STRING_TYPE>
STRING_TYPE& ltrim(STRING_TYPE &s) 
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(safe_isspace))));
	return s;
}

template <typename STRING_TYPE>
STRING_TYPE& trim(STRING_TYPE &s) 
{
	rtrim(s);
	ltrim(s);
	return s;
}

/* Taken from SDL */
#if UNICODE
#define StringToUTF8(S) SDL_iconv_string("UTF-8", "UTF-16LE", (char *)(S), (SDL_wcslen(S)+1)*sizeof(WCHAR))
#define UTF8ToString(S) (WCHAR *)SDL_iconv_string("UTF-16LE", "UTF-8", (char *)(S), SDL_strlen(S)+1)
#else
#define StringToUTF8(S) SDL_iconv_string("UTF-8", "ASCII", (char *)(S), (SDL_strlen(S)+1))
#define UTF8ToString(S) SDL_iconv_string("ASCII", "UTF-8", (char *)(S), SDL_strlen(S)+1)
#endif

// Extracts resolution dimensions in the standard format (e.g. 1920x1080) from a string.
bool ExtractResolution(const tstring& str, int* w, int* h);

int strpos(const TCHAR* haystack, const TCHAR* needle);
#if defined(UNICODE)
int strpos(const char * haystack, const char * needle); 
#endif

#endif
