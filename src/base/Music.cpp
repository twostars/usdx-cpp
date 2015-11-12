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
#include "Music.h"
#include "PathUtils.h"
#include "Ini.h"

initialiseSingleton(SoundLibrary);

SoundLibrary::SoundLibrary()
{
	Load();
}

void SoundLibrary::Load()
{
	#define SOUND_PATH(mp3) \
		((SoundPath / mp3).generic_string().c_str())

	_sounds[SoundStart] = Mix_LoadWAV(SOUND_PATH("Common start.wav"));
	_sounds[SoundBack] = Mix_LoadWAV(SOUND_PATH("Common back.wav"));
	_sounds[SoundSwoosh] = Mix_LoadWAV(SOUND_PATH("menu swoosh.wav"));
	_sounds[SoundChange] = Mix_LoadWAV(SOUND_PATH("select music change music 50.wav"));
	_sounds[SoundOption] = Mix_LoadWAV(SOUND_PATH("option change col.wav"));
	_sounds[SoundClick] = Mix_LoadWAV(SOUND_PATH("rimshot022b.wav"));
	_sounds[SoundApplause] = Mix_LoadWAV(SOUND_PATH("Applause.wav"));
	_music = Mix_LoadMUS(SOUND_PATH("background track.mp3"));

	#undef SOUND_PATH
}

void SoundLibrary::Unload()
{
	for (size_t i = 0; i < SoundCount; i++)
		Mix_FreeChunk(_sounds[i]);

	Mix_FreeMusic(_music);
}

void SoundLibrary::StartBgMusic()
{
	if (sIni.BackgroundMusic == eSwitch::Off)
		return;

	if (Mix_PausedMusic() == 1)
		Mix_ResumeMusic();
	else
		Mix_PlayMusic(_music, 0);
}

void SoundLibrary::PauseBgMusic()
{
	Mix_PauseMusic();
}

void SoundLibrary::PlaySound(SoundType soundType)
{
	Mix_PlayChannel(-1, _sounds[soundType], 0);
}

SoundLibrary::~SoundLibrary()
{
	Unload();
}
