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

#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H
#pragma once

class BaseException : public std::exception
{
public:
	BaseException()
	{
	}

	void SetMessage(const char * format, va_list args)
	{
		char buffer[1024];
		_vsnprintf(buffer, 1024, format, args);
		_message = buffer;
	}

	const char * what() const
	{
		return _message.c_str();
	}

private:
	std::string _message;
};


#define DECLARE_EXCEPTION(name, base) \
class name : public base \
{ \
public: \
	name(const char * format, ...) : base() \
	{ \
		va_list args; \
		va_start(args, format); \
		SetMessage(format, args); \
		va_end(args); \
	} \
}

DECLARE_EXCEPTION(CriticalException,       BaseException);
DECLARE_EXCEPTION(NotImplementedException, BaseException);

#endif
