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
#include "CommandLine.h"

#include "../lib/simpleopt/SimpleOpt.h"
#include "../lib/simpleopt/SimpleGlob.h"

using namespace boost::filesystem;

extern path	SongPath;
extern path	ConfigFile;
extern path	ScoreFile;

enum
{
	OPT_HELP = 0, 
	OPT_DEBUG, OPT_BENCHMARK, OPT_NOLOG, 
	OPT_FULLSCREEN, OPT_WINDOWED, 
	OPT_JOYPAD,
	OPT_DEPTH, OPT_SCREENS, 
	OPT_LANGUAGE, OPT_RESOLUTION,
	OPT_SONGPATH, OPT_CONFIGFILE, OPT_SCOREFILE
};

CSimpleOpt::SOption g_rgOptions[] =
{
	{ OPT_HELP,			_T("-?"),			SO_NONE },
	{ OPT_HELP,			_T("-h"),			SO_NONE },
	{ OPT_HELP,			_T("-help"),		SO_NONE },
	{ OPT_HELP,			_T("--help"),		SO_NONE },
	{ OPT_DEBUG,		_T("-d"),			SO_NONE },
	{ OPT_DEBUG,		_T("-debug"),		SO_NONE },
	{ OPT_DEBUG,		_T("--debug"),		SO_NONE },
	{ OPT_BENCHMARK,	_T("-benchmark"),	SO_NONE },
	{ OPT_BENCHMARK,	_T("--benchmark"),	SO_NONE },
	{ OPT_NOLOG,		_T("-nolog"),		SO_NONE },
	{ OPT_NOLOG,		_T("--nolog"),		SO_NONE },
	{ OPT_FULLSCREEN,	_T("-fullscreen"),	SO_NONE },
	{ OPT_FULLSCREEN,	_T("--fullscreen"),	SO_NONE },
	{ OPT_WINDOWED,		_T("-window"),		SO_NONE }, /* legacy */
	{ OPT_WINDOWED,		_T("--window"),		SO_NONE }, /* legacy */
	{ OPT_WINDOWED,		_T("-windowed"),	SO_NONE },
	{ OPT_WINDOWED,		_T("--windowed"),	SO_NONE },
	{ OPT_JOYPAD,		_T("-joypad"),		SO_NONE },
	{ OPT_JOYPAD,		_T("--joypad"),		SO_NONE },
	{ OPT_DEPTH,		_T("-depth"),		SO_OPT },
	{ OPT_DEPTH,		_T("--depth"),		SO_OPT },
	{ OPT_SCREENS,		_T("-screens"),		SO_OPT },
	{ OPT_SCREENS,		_T("--screens"),	SO_OPT },
	{ OPT_LANGUAGE,		_T("-language"),	SO_OPT },
	{ OPT_LANGUAGE,		_T("--language"),	SO_OPT },
	{ OPT_RESOLUTION,	_T("-resolution"),	SO_OPT },
	{ OPT_RESOLUTION,	_T("--resolution"),	SO_OPT },
	{ OPT_SONGPATH,		_T("-songpath"),	SO_OPT },
	{ OPT_SONGPATH,		_T("--songpath"),	SO_OPT },
	{ OPT_CONFIGFILE,	_T("-configfile"),	SO_OPT },
	{ OPT_CONFIGFILE,	_T("--configfile"),	SO_OPT },
	{ OPT_SCOREFILE,	_T("-scorefile"),	SO_OPT },
	{ OPT_SCOREFILE,	_T("--scorefile"),	SO_OPT },

	SO_END_OF_OPTIONS
};

CMDParams::CMDParams() :
	Debug(false), Benchmark(false), NoLog(false), Joypad(false),
	ScreenMode(scmDefault), Depth(32), Screens(1)
{
}

static const TCHAR * GetLastErrorText(int nError) 
{
	switch (nError)
	{
	case SO_SUCCESS:            return _T("Success");
	case SO_OPT_INVALID:        return _T("Unrecognized option");
	case SO_OPT_MULTIPLE:       return _T("Option matched multiple strings");
	case SO_ARG_INVALID:        return _T("Option does not accept argument");
	case SO_ARG_INVALID_TYPE:   return _T("Invalid argument format");
	case SO_ARG_MISSING:        return _T("Required argument is missing");
	case SO_ARG_INVALID_DATA:   return _T("Invalid argument data");
	default:                    return _T("Unknown error");
	}
}

void CMDParams::Load(int argc, TCHAR ** argv)
{
	CSimpleOpt args(argc, argv, g_rgOptions, SO_O_NOERR | SO_O_EXACT | SO_O_ICASE);

	while (args.Next())
	{
		if (args.LastError() != SO_SUCCESS)
		{
			_tprintf(_T("%s: '%s' (use --help to get command line help)\n"),
				GetLastErrorText(args.LastError()), args.OptionText());
			continue;
		}

		switch (args.OptionId())
		{
		case OPT_HELP:
			ShowUsage();
			return;

		case OPT_DEBUG:
			Debug = true;
			break;

		case OPT_BENCHMARK:
			Benchmark = true;
			break;

		case OPT_NOLOG:
			NoLog = true;
			break;

		case OPT_FULLSCREEN:
			ScreenMode = scmFullscreen;
			break;

		case OPT_WINDOWED:
			ScreenMode = scmWindowed;
			break;

		case OPT_JOYPAD:
			Joypad = true;
			break;

		case OPT_DEPTH:
			Depth = _ttoi(args.OptionArg());
			if (Depth != 16 && Depth != 32)
				Depth = 32;
			break;

		case OPT_SCREENS:
			Screens = _ttoi(args.OptionArg());
			if (Screens < 1)
				Screens = 1;
			break;

		case OPT_LANGUAGE:
			Language = args.OptionArg();
			break;

		case OPT_RESOLUTION:
			Resolution = args.OptionArg();
			break;

		case OPT_SONGPATH:
			SongPath = args.OptionArg();
			break;

		case OPT_CONFIGFILE:
			ConfigFile = args.OptionArg();
			break;

		case OPT_SCOREFILE:
			ScoreFile = args.OptionArg();
			break;
		}
	}
}

void CMDParams::ShowUsage()
{
	_tprintf(
		_T("Usage: ultrastardeluxe.exe [OPTIONS]\n")
		_T("\n")
		_T("-d  -debug  --debug       Enables debug.\n")
		_T("-benchmark  --benchmark   Enables performance benchmarking.\n")
		_T("-nolog      --nolog       Disables logging.\n")
		_T("-fullscreen --fullscreen  Starts in fullscreen mode.\n")
		_T("-windowed   --windowed    Starts in windowed mode.\n")
		_T("-joypad     --joypad      Enables joypad support.\n")
		_T("-depth      --depth       Sets the screen depth (set to either 16 or 32).\n")
		_T("-screens    --screens     Sets the number of screens to use.\n")
		_T("-language   --language    Sets the language to use.\n")
		_T("-resolution --resolution  Sets the resolution to use.\n")
		_T("-songpath   --songpath    Sets the song path to use.\n")
		_T("-configfile --configfile  Sets the config file to use.\n")
		_T("-scorefile  --scorefile   Sets the score file to use.\n")
		_T("\n")
		_T("-?  -h  -help  --help     Output this help.\n")
		_T("\n")
	);
}
