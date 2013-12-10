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

#ifndef _TEXT_ENCODING_H
#define _TEXT_ENCODING_H
#pragma once

#define IMPROVED_ENUM_NAME  Encoding
#define IMPROVED_ENUM_LIST  \
	ENUMITEM(Locale) /* current locale */                                               \
    ENUMITEM(UTF8)   /* UTF-8 */                                                        \
    ENUMITEM(CP1250) /* Windows-1250 Central/Eastern Europe (used by UltraStar) */      \
    ENUMITEM(CP1252) /* Windows-1252 Western Europe (used by UltraStar Deluxe < 1.1) */ \
    ENUMITEM(Auto)   /* try to match the w3c regex and decode as unicode on match and as fallback if not */

#include <DefineImprovedEnum.h>

#endif
