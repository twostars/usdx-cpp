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

#ifndef _COMMON_H
#define _COMMON_H
#pragma once

class Common
{
public:
	static void SetDefaultNumericLocale();
	static void RestoreNumericLocale();

private:
	static char * s_previousNumLocale;
};

#define GET_INDEX(arr, value) GetArrayIndex(arr, sizeof(arr) / sizeof(arr[0]), value)
#define GET_INDEX_SIZE(arr, value, size) GetArrayIndex(arr, size, value)

template <typename T, typename T2>
int GetArrayIndex(T arr, int length, T2 value)
{
	for (int i = 0; i < length; i++)
	{
		if (arr[i] == value)
			return i;
	}
	return -1;
}

#endif
