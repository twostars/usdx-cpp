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

#include "stdafx.h"

// Extracts resolution dimensions in the standard format (e.g. 1920x1080) from a string.
bool ExtractResolution(const tstring& str, int* w, int* h)
{
	const TCHAR separator = _T('x');

	assert(w != NULL && h != NULL);

	// Make a lowercase copy of the string to check against.
	tstring strCopy = str;
	strtolower(strCopy);

	// Search the lowercase string for the resolution separator.
	size_t pos = strCopy.find(separator);
	if (pos == tstring::npos)
		return false;

	// Read from the start until the first non-character digit (i.e. x)
	*w = _ttoi(str.c_str());

	// Read from the separator to the next non-character digit (e.g. null-terminator).
	// Note that it ignores preleading spaces.
	*h = _ttoi(str.c_str() + pos + 1);

	return true;
}

int strpos(const TCHAR* haystack, const TCHAR* needle)
{
	TCHAR *p = _tcsstr((TCHAR *)haystack, (TCHAR *)needle);
	if (p)
		return p - haystack;

	return -1;
}

#if defined(UNICODE)
int strpos(const char * haystack, char * needle)
{
	char * p = strstr((char *)haystack, (char *)needle);
	if (p)
		return p - haystack;

	return NULL;
}
#endif
