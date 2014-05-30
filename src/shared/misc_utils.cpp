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

bool IsInStringArray(const TCHAR * needle, const TCHAR ** haystackArray)
{
	if (needle == NULL || *needle == 0
		|| haystackArray == NULL || *haystackArray == 0)
		return false;

	uint32 i = 0;
	while (haystackArray[i] != NULL)
	{
		if (_tcscmp(haystackArray[i], needle) == 0)
			return true;

		i++;
	}

	return false;
}

bool IsInStringArrayI(const TCHAR * needle, const TCHAR ** haystackArray)
{
	if (needle == NULL || *needle == 0
		|| haystackArray == NULL || *haystackArray == 0)
		return false;

	uint32 i = 0;
	while (haystackArray[i] != NULL)
	{
		if (TSTRCASECMP(haystackArray[i], needle) == 0)
			return true;

		i++;
	}

	return false;
}

uint32 GetListIndex(OptionList& list, const tstring & val)
{
	// TODO: This is not especially efficient, this needs to be reworked.
	for (size_t i = 0; i < list.size(); i++)
	{
		if (val == list[i])
			return (uint32) i;
	}

	return 0;
}
