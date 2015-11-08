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

CSimpleOptA::SOption g_rgOptions[] =
{
	{ OPT_HELP,			"-?",			SO_NONE },
	{ OPT_HELP,			"-h",			SO_NONE },
	{ OPT_HELP,			"-help",		SO_NONE },
	{ OPT_HELP,			"--help",		SO_NONE },
	{ OPT_DEBUG,		"-d",			SO_NONE },
	{ OPT_DEBUG,		"-debug",		SO_NONE },
	{ OPT_DEBUG,		"--debug",		SO_NONE },
	{ OPT_BENCHMARK,	"-benchmark",	SO_NONE },
	{ OPT_BENCHMARK,	"--benchmark",	SO_NONE },
	{ OPT_NOLOG,		"-nolog",		SO_NONE },
	{ OPT_NOLOG,		"--nolog",		SO_NONE },
	{ OPT_FULLSCREEN,	"-fullscreen",	SO_NONE },
	{ OPT_FULLSCREEN,	"--fullscreen",	SO_NONE },
	{ OPT_WINDOWED,		"-window",		SO_NONE }, /* legacy */
	{ OPT_WINDOWED,		"--window",		SO_NONE }, /* legacy */
	{ OPT_WINDOWED,		"-windowed",	SO_NONE },
	{ OPT_WINDOWED,		"--windowed",	SO_NONE },
	{ OPT_JOYPAD,		"-joypad",		SO_NONE },
	{ OPT_JOYPAD,		"--joypad",		SO_NONE },
	{ OPT_DEPTH,		"-depth",		SO_OPT },
	{ OPT_DEPTH,		"--depth",		SO_OPT },
	{ OPT_SCREENS,		"-screens",		SO_OPT },
	{ OPT_SCREENS,		"--screens",	SO_OPT },
	{ OPT_LANGUAGE,		"-language",	SO_OPT },
	{ OPT_LANGUAGE,		"--language",	SO_OPT },
	{ OPT_RESOLUTION,	"-resolution",	SO_OPT },
	{ OPT_RESOLUTION,	"--resolution",	SO_OPT },
	{ OPT_SONGPATH,		"-songpath",	SO_OPT },
	{ OPT_SONGPATH,		"--songpath",	SO_OPT },
	{ OPT_CONFIGFILE,	"-configfile",	SO_OPT },
	{ OPT_CONFIGFILE,	"--configfile",	SO_OPT },
	{ OPT_SCOREFILE,	"-scorefile",	SO_OPT },
	{ OPT_SCOREFILE,	"--scorefile",	SO_OPT },

	SO_END_OF_OPTIONS
};

CMDParams::CMDParams() :
	Debug(false), Benchmark(false), NoLog(false), Joypad(false),
	ScreenMode(scmDefault), Depth(32), Screens(1)
{
}

static const char * GetLastErrorText(int nError) 
{
	switch (nError)
	{
	case SO_SUCCESS:            return "Success";
	case SO_OPT_INVALID:        return "Unrecognized option";
	case SO_OPT_MULTIPLE:       return "Option matched multiple strings";
	case SO_ARG_INVALID:        return "Option does not accept argument";
	case SO_ARG_INVALID_TYPE:   return "Invalid argument format";
	case SO_ARG_MISSING:        return "Required argument is missing";
	case SO_ARG_INVALID_DATA:   return "Invalid argument data";
	default:                    return "Unknown error";
	}
}

void CMDParams::Load(int argc, char ** argv)
{
	CSimpleOptA args(argc, argv, g_rgOptions, SO_O_NOERR | SO_O_EXACT | SO_O_ICASE);

	while (args.Next())
	{
		if (args.LastError() != SO_SUCCESS)
		{
			printf("%s: '%s' (use --help to get command line help)\n",
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
			Depth = atoi(args.OptionArg());
			if (Depth != 16 && Depth != 32)
				Depth = 32;
			break;

		case OPT_SCREENS:
			Screens = atoi(args.OptionArg());
			if (Screens < 1)
				Screens = 1;
			break;

		case OPT_LANGUAGE:
			LanguageName = args.OptionArg();
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
	printf(
		"Usage: ultrastardeluxe.exe [OPTIONS]\n"
		"\n"
		"-d  -debug  --debug       Enables debug.\n"
		"-benchmark  --benchmark   Enables performance benchmarking.\n"
		"-nolog      --nolog       Disables logging.\n"
		"-fullscreen --fullscreen  Starts in fullscreen mode.\n"
		"-windowed   --windowed    Starts in windowed mode.\n"
		"-joypad     --joypad      Enables joypad support.\n"
		"-depth      --depth       Sets the screen depth (set to either 16 or 32).\n"
		"-screens    --screens     Sets the number of screens to use.\n"
		"-language   --language    Sets the language to use.\n"
		"-resolution --resolution  Sets the resolution to use.\n"
		"-songpath   --songpath    Sets the song path to use.\n"
		"-configfile --configfile  Sets the config file to use.\n"
		"-scorefile  --scorefile   Sets the score file to use.\n"
		"\n"
		"-?  -h  -help  --help     Output this help.\n"
		"\n"
	);
}
