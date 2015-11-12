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
#include "../base/Graphic.h"
#include "../base/Language.h"
#include "../base/Ini.h"
#include "../base/Themes.h"
#include "../base/Music.h"
#include "../menu/Menu.h"
#include "ScreenPartyPlayer.h"
#include "ScreenPartyRounds.h"
#include "ScreenPartyOptions.h"
#include "ScreenPopup.h"

static const std::string ITeams[] = { "2", "3" };
static const std::string IPlayers_[] = { "1", "2", "3", "4" };

ScreenPartyPlayer::ScreenPartyPlayer() : Menu()
{
	ThemePartyPlayer * theme = sThemes.PartyPlayer;

	LoadFromTheme(theme);

	theme->SelectTeams.OneItemOnly = true;
	theme->SelectTeams.ShowArrows = true;
	_selectTeams = AddSelectSlide(theme->SelectTeams, &_teamCount, ITeams, SDL_arraysize(ITeams));

	for (size_t teamNo = 0; teamNo < MAX_TEAMS; teamNo++)
	{
		ThemePartyPlayer::Team& team = theme->Teams[teamNo];
		int teamNameButton = AddButton(team.TeamName);
		_teamNameButtons.push_back(teamNameButton);

		team.SelectPlayers.OneItemOnly = true;
		team.SelectPlayers.ShowArrows = true;
		_selectPlayers[teamNo] = AddSelectSlide(team.SelectPlayers, &_playerCount[teamNo], IPlayers_, SDL_arraysize(IPlayers_));

		for (size_t playerNo = 0; playerNo < MAX_PLAYERS_PER_TEAM; playerNo++)
		{
			int playerNameButton = AddButton(team.PlayerName[playerNo]);
			_playerNameButtons.push_back(playerNameButton);
		}

		_playerCount[teamNo] = 0;
	}

	_teamCount = 0;
	SetInteraction(0);
}

void ScreenPartyPlayer::OnShow()
{
	for (size_t i = 0; i < MAX_PLAYERS_PER_TEAM * MAX_TEAMS; i++)
	{
		int index = _playerNameButtons[i];
		MenuButton& playerNameButton = Buttons[index];
		playerNameButton.Texts[0].SetText(sIni.Name[i]);
	}

	for (size_t i = 0; i < MAX_TEAMS; i++)
	{
		int index = _teamNameButtons[i];
		MenuButton& teamNameButton = Buttons[index];
		teamNameButton.Texts[0].SetText(sIni.NameTeam[i]);
	}

	UpdateInterface();
	Menu::OnShow();
}

bool ScreenPartyPlayer::ParseInput(Uint32 pressedKey, SDL_Keycode keyCode, bool pressedDown)
{
	Uint16 modState = 0;

	if (pressedDown)
		modState = SDL_GetModState() & (KMOD_SHIFT | KMOD_CTRL | KMOD_ALT);

	MenuInteract& interaction = Interactions[SelInteraction];
	if (interaction.Type == itButton)
	{
		MenuButton& button = Buttons[interaction.Num];

		// Check special keys
		if (keyCode >= SDLK_F1 && keyCode <= SDLK_F12)
		{
			size_t index = keyCode - SDLK_F1;
			if (modState & KMOD_ALT)
				sIni.NameTemplate[index] = button.Texts[0].GetText();
			else
				button.Texts[0].SetText(sIni.NameTemplate[index]);
		}
		else if (keyCode == SDLK_BACKSPACE)
		{
			button.Texts[0].DeleteLastLetter();
		}
	}

	switch (keyCode)
	{
	case SDLK_ESCAPE:
		sIni.SaveProfileSettings();
		FadeTo(UIPartyOptions, SoundBack);
		break;

	case SDLK_RETURN:
		UpdateParty();
		break;

	case SDLK_DOWN:
		IntNext();
		break;

	case SDLK_UP:
		IntPrev();
		break;

	case SDLK_RIGHT:
	case SDLK_LEFT:
		if (interaction.Type == itSelectSlide)
		{
			bool r = false;

			if (keyCode == SDLK_RIGHT)
				r = InteractInc();
			else
				r = InteractDec();

			if (r)
				sSoundLib.PlaySound(SoundOption);

			UpdateInterface();
		}
		break;
	}
	return true;
}

bool ScreenPartyPlayer::ParseTextInput(SDL_Event * event)
{
	MenuInteract& interaction = Interactions[SelInteraction];
	if (interaction.Type == itButton)
	{
		MenuText& menuText = Buttons[interaction.Num].Texts[0];
		if (event->type == SDL_TEXTINPUT)
			menuText.SetText(menuText.GetText() + event->text.text);
	}

	return true;
}

void ScreenPartyPlayer::UpdateInterface()
{
	SelectSlides[_selectPlayers[2]].Visible = (_teamCount == 1);

	size_t btn = 0;
	for (size_t teamNo = 0; teamNo < MAX_TEAMS; teamNo++)
	{
		if (_teamCount + 1 >= teamNo)
		{
			Buttons[_teamNameButtons[teamNo]].Visible = true;

			for (size_t playerNo = 0; playerNo < MAX_PLAYERS_PER_TEAM; playerNo++)
				Buttons[_playerNameButtons[btn + playerNo]].Visible = (_playerCount[teamNo] >= playerNo);
		}
		else
		{
			Buttons[_teamNameButtons[teamNo]].Visible = false;

			for (size_t playerNo = 0; playerNo < MAX_PLAYERS_PER_TEAM; playerNo++)
				Buttons[_playerNameButtons[btn + playerNo]].Visible = false;
		}

		btn += MAX_PLAYERS_PER_TEAM;
	}
}

void ScreenPartyPlayer::UpdateParty()
{
	size_t playerIndex = 0;
	for (size_t teamNo = 0; teamNo < _teamCount + 1; teamNo++)
	{
		int index = _teamNameButtons[teamNo];
		MenuButton& teamNameButton = Buttons[index];
		// Party.AddTeam(teamNameButton.Texts[0].GetText()); // TODO

		for (size_t playerNo = 0; playerNo < _playerCount[teamNo]; playerNo++)
		{
			MenuButton& playerNameButton = Buttons[playerIndex + playerNo];
			// Party.AddPlayer(teamNo, playerNameButton.Texts[0].GetText()); // TODO
		}
		playerIndex += MAX_PLAYERS_PER_TEAM;
	}

	// Mode for current player setup available
	// if (Party.ModesAvailable)
	// {
		FadeTo(UIPartyRounds, SoundStart);
	// }
	// No mode available for current player setup
	// else
	// {
	//	UIPopupError->ShowPopup(__T("ERROR_NO_MODES_FOR_CURRENT_SETUP"));
	//	Party.Clear();
	// }
}

void ScreenPartyPlayer::IntNext()
{
	while (1)
	{
		InteractNext();
		if (((Interactions[SelInteraction].Type == itSelectSlide)
			&& SelectSlides[Interactions[SelInteraction].Num].Visible)
			|| (Buttons[Interactions[SelInteraction].Num].Visible))
			break;
	}
}

void ScreenPartyPlayer::IntPrev()
{
	while (1)
	{
		InteractPrev();
		if (((Interactions[SelInteraction].Type == itSelectSlide)
			&& SelectSlides[Interactions[SelInteraction].Num].Visible)
			|| (Buttons[Interactions[SelInteraction].Num].Visible))
			break;
	}
}
