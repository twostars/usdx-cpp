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
#include <clocale>

char * Common::s_previousNumLocale;

// In Linux and maybe MacOSX some units (like cwstring) call setlocale(LC_ALL, '')
// to set the language/country specific locale (e.g. charset) for this application.
// Unfortunately, LC_NUMERIC is set by this call too.
// It defines the decimal-separator and other country-specific numeric settings.
// This parameter is used by the C string-to-float parsing functions atof() and strtod().
// After changing LC_NUMERIC some external C-based libs (like projectM) are not
// able to parse strings correctly
// (e.g. in Germany "0.9" is not recognized as a valid number anymore but "0,9" is).
// So we reset the numeric settings to the default ('C').
// TODO:
// - Check if this is needed in MacOSX (at least the locale is set in cwstring)
// - Find out which libs are concerned by this problem.
//   If only projectM is concerned by this problem set and restore the numeric locale
//   for each call to projectM instead of changing it globally.
void Common::SetDefaultNumericLocale()
{
	s_previousNumLocale = setlocale(LC_NUMERIC, NULL);
	setlocale(LC_NUMERIC, "C");
}

void Common::RestoreNumericLocale()
{
	setlocale(LC_NUMERIC, s_previousNumLocale);
}
