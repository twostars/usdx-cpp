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

#ifndef _LOG_H
#define _LOG_H
#pragma once

#define LOG_FILE			"error.log"
#define MAX_BENCHMARK_COUNT	32

enum LogLevel
{
	LOG_LEVEL_MAX          = 60,
	LOG_LEVEL_DEBUG_MAX    = LOG_LEVEL_MAX - 1,
	LOG_LEVEL_DEBUG        = 50,
	LOG_LEVEL_INFO_MAX     = LOG_LEVEL_DEBUG - 1,
	LOG_LEVEL_INFO         = 40,
	LOG_LEVEL_STATUS_MAX   = LOG_LEVEL_INFO - 1,
	LOG_LEVEL_STATUS       = 30,
	LOG_LEVEL_WARN_MAX     = LOG_LEVEL_STATUS - 1,
	LOG_LEVEL_WARN         = 20,
	LOG_LEVEL_ERROR_MAX    = LOG_LEVEL_WARN - 1,
	LOG_LEVEL_ERROR        = 10,
	LOG_LEVEL_CRITICAL_MAX = LOG_LEVEL_ERROR - 1,
	LOG_LEVEL_CRITICAL     =  0,
	LOG_LEVEL_NONE         = -1,

	// define level that Log(File)Level is initialized with
	LOG_LEVEL_DEFAULT      = LOG_LEVEL_MAX, // TODO: Revert this back to LOG_LEVEL_WARN once the initial port's done
	LOG_FILE_LEVEL_DEFAULT = LOG_LEVEL_ERROR
};

class Log : public Singleton<Log>
{
public:
	Log();

	void BenchmarkStart(int benchmarkNo);
	void BenchmarkEnd(int benchmarkNo);
	void Benchmark(int benchmarkNo, const char * message, ...);

	void Msg(int level, const char * message);
	void Msg(int level, const char * context, const char * message);
	void Debug(const char * context, const char * message, ...);
	void Info(const char * context, const char * message, ...);
	void Status(const char * context, const char * message, ...);
	void Warn(const char * context, const char * message, ...);
	void Error(const char * message);
	void Error(const char * context, const char * message, ...);

	// critical error (halt & messagebox)
	void Critical(const char * message);
	void Critical(const char * context, const char * message, ...);

	void Voice(int soundNo);
	void Buffer(const char * buffer, const size_t length, const path& filename);

	void LogToFile(const char * message);

	void DebugWriteLn(const char * message);

	INLINE void SetLogLevel(int level) { _logLevel = level; }
	INLINE int GetLogLevel() { return _logLevel; }

	INLINE void SetLogFileLevel(int level) { _logFileLevel = level; }
	INLINE int GetLogFileLevel() { return _logFileLevel; }

	~Log();

private:
	int		_logLevel;
	int		_logFileLevel;

	FILE *	_logFile;
	FILE *	_benchmarkFile;

	float	_benchmarkTimeStart [MAX_BENCHMARK_COUNT];
	float	_benchmarkTimeLength[MAX_BENCHMARK_COUNT];
};

#define sLog (Log::getSingleton())

#endif
