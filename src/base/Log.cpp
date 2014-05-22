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
#include <time.h>

#include "Log.h"
#include "CommandLine.h"
#include "Time.h"
#include "Platform.h"

extern Time USTime;
extern CMDParams Params;

initialiseSingleton(Log);

#define BUILD_VA_BUFFER(formatBuffer, formatArg, buffer, bufferSize) \
	TCHAR buffer[bufferSize]; \
	va_list ap; \
	va_start(ap, formatArg); \
	_vsntprintf(buffer, bufferSize, formatBuffer, ap); \
	va_end(ap)

Log::Log() :
	_logFile(NULL), _benchmarkFile(NULL),
	_logLevel(LOG_LEVEL_DEFAULT), _logFileLevel(LOG_FILE_LEVEL_DEFAULT)
{
	memset(&_benchmarkTimeStart,  0, sizeof(_benchmarkTimeStart));
	memset(&_benchmarkTimeLength, 0, sizeof(_benchmarkTimeLength));
}

void Log::BenchmarkStart(int benchmarkNo)
{
	assert(benchmarkNo >= 0 && benchmarkNo < MAX_BENCHMARK_COUNT);
	_benchmarkTimeStart[benchmarkNo] = USTime.GetTime();
}

void Log::BenchmarkEnd(int benchmarkNo)
{
	assert(benchmarkNo >= 0 && benchmarkNo < MAX_BENCHMARK_COUNT);
	_benchmarkTimeLength[benchmarkNo] = USTime.GetTime() - _benchmarkTimeStart[benchmarkNo];
}

void Log::Benchmark(int benchmarkNo, const TCHAR * message, ...)
{
	BUILD_VA_BUFFER(message, message, buffer, 1024);
	// TODO
}

void Log::Msg(int level, const TCHAR * message)
{
	TCHAR buffer[1024] = {0};

	if (level <= GetLogLevel()
		|| level <= GetLogFileLevel())
	{
		if (level < LOG_LEVEL_CRITICAL_MAX)
			_sntprintf(buffer, 1024, _T("CRITICAL: %s"), message);
		else if (level < LOG_LEVEL_ERROR_MAX)
			_sntprintf(buffer, 1024, _T("ERROR:    %s"), message);
		else if (level < LOG_LEVEL_WARN_MAX)
			_sntprintf(buffer, 1024, _T("WARN:     %s"), message);
		else if (level < LOG_LEVEL_STATUS_MAX)
			_sntprintf(buffer, 1024, _T("STATUS:   %s"), message);
		else if (level < LOG_LEVEL_INFO_MAX)
			_sntprintf(buffer, 1024, _T("INFO:     %s"), message);
		else
			_sntprintf(buffer, 1024, _T("DEBUG:    %s"), message);

		if (level <= GetLogLevel())
			DebugWriteLn(buffer);

		if (level <= GetLogFileLevel())
			LogToFile(buffer);
	}

	if (level <= LOG_LEVEL_CRITICAL_MAX)
	{
		Platform::ShowMessage(message, mtError);
		throw CriticalException(message);
	}
}

void Log::Msg(int level, const TCHAR * context, const TCHAR * message)
{
	TCHAR buffer[1024] = {0};
	_sntprintf(buffer, 1024, _T("[%s] %s"), context, message);
	Msg(level, buffer);
}

void Log::Debug(const TCHAR * context, const TCHAR * message, ...)
{
	BUILD_VA_BUFFER(message, message, buffer, 1024);
	Msg(LOG_LEVEL_DEBUG, context, buffer);
}

void Log::Info(const TCHAR * context, const TCHAR * message, ...)
{
	BUILD_VA_BUFFER(message, message, buffer, 1024);
	Msg(LOG_LEVEL_INFO, context, buffer);
}

void Log::Status(const TCHAR * context, const TCHAR * message, ...)
{
	BUILD_VA_BUFFER(message, message, buffer, 1024);
	Msg(LOG_LEVEL_STATUS, context, buffer);
}

void Log::Warn(const TCHAR * context, const TCHAR * message, ...)
{
	BUILD_VA_BUFFER(message, message, buffer, 1024);
	Msg(LOG_LEVEL_WARN, context, buffer);
}

void Log::Error(const TCHAR * message)
{
	Msg(LOG_LEVEL_ERROR, message);
}

void Log::Error(const TCHAR * context, const TCHAR * message, ...)
{
	BUILD_VA_BUFFER(message, message, buffer, 1024);
	Msg(LOG_LEVEL_ERROR, context, buffer);
}

void Log::Critical(const TCHAR * message)
{
	Msg(LOG_LEVEL_CRITICAL, message);
}

void Log::Critical(const TCHAR * context, const TCHAR * message, ...)
{
	BUILD_VA_BUFFER(message, message, buffer, 1024);
	Msg(LOG_LEVEL_CRITICAL, context, buffer);
}

void Log::Voice(int soundNo)
{
	// TODO
}

void Log::Buffer(const TCHAR * buffer, const size_t length, const boost::filesystem::path& filename)
{
	FILE * fp = _tfopen(filename.native().c_str(), _T("a"));
	if (fp == NULL)
	{
		Error(_T("LogBuffer"), _T("Failed to log buffer to file."));
		return;
	}

	fwrite(buffer, length, 1, fp);
	fclose(fp);
}

void Log::LogToFile(const TCHAR * message)
{
	if (Params.NoLog)
		return;

	if (_logFile == NULL)
	{
		_logFile = _tfopen(LOG_FILE, _T("a"));
		if (_logFile == NULL)
		{
			DebugWriteLn(_T("Failed to write to log file."));
			return;
		}

		// If log file is opened, write date/time to file
		time_t unixTime = time(NULL);
		tm localTime = *localtime(&unixTime);

		_ftprintf(_logFile, _T("Logging started\n"));
		_ftprintf(_logFile, _T("Date: %02u/%02u/%04u Time: %02u:%02u\n"),
			localTime.tm_mday, localTime.tm_mon + 1, 1900 + localTime.tm_year,
			localTime.tm_hour, localTime.tm_min);
		_ftprintf(_logFile, _T("-------------------\n"));
	}

	_ftprintf(_logFile, _T("%s\n"), message);
	fflush(_logFile);
}

void Log::DebugWriteLn(const TCHAR * message)
{
	if (Params.Debug)
		_tprintf(_T("%s\n"), message);
}

Log::~Log()
{
	if (_benchmarkFile != NULL)
	{
		fclose(_benchmarkFile);
		_benchmarkFile = NULL;
	}

	if (_logFile != NULL)
	{
		fclose(_logFile);
		_logFile = NULL;
	}
}
