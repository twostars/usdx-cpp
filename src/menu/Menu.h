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

#ifndef _MENU_H
#define _MENU_H
#pragma once

#include "../base/Texture.h"

typedef void(*fnMenuValueChanged)(Uint32 oldValue, Uint32 newValue);

#include "MenuBackground.h"
#include "MenuText.h"
#include "MenuInteract.h"
#include "MenuButton.h"
#include "MenuStatic.h"
#include "MenuSelectSlide.h"
#include "MenuButtonCollection.h"

#include "../base/ThemeDefines.h"

enum
{
	MENU_MDOWN	= 8,
	MENU_MUP	= 0
};

enum
{
	pmMove		= 1,
	pmClick		= 2,
	pmUnclick	= 3
};

enum FadeType
{
	fBlack		= 0,
	fWhite		= 1
};

class MenuBackground;
class Menu
{
public:
	std::vector<MenuText> Texts;
	std::vector<MenuStatic> Statics;

	int MouseX, MouseY;

	FadeType Fade;
	bool ShowFinish; // true if there is no fade
	bool RightMbESC; // true to simulate ESC keypress when RMB is pressed

	Menu();

	// Interaction
	void AddInteraction(InteractionType type, int num);
	virtual void SetInteraction(int num);
	virtual void OnInteraction();

	// load background, texts, statics and button collections from ThemeBasic
	INLINE void LoadFromTheme(const ThemeBasic* themeBasic)
	{
		assert(themeBasic != NULL);
		LoadFromTheme(*themeBasic);
	}

	void LoadFromTheme(const ThemeBasic& themeBasic);

	// Button collections
	void PrepareButtonCollections(const AThemeButtonCollection& collections);
	void AddButtonCollection(const ThemeButtonCollection& collection, Uint8 num);

	// Background
	void AddBackground(const ThemeBackground& themedSettings);
	template <class BG_TYPE>
	bool TryBGCreate(const ThemeBackground& themedSettings)
	{
		try
		{
			assert(Background == NULL);
			Background = new BG_TYPE(&themedSettings);
			return true;
		}
		catch (const MenuBackgroundException& e)
		{
			LogBackgroundException(e);
			delete Background;
			Background = NULL;
			return false;
		}
	}
	void LogBackgroundException(const MenuBackgroundException& e);

	// Static
	int AddStatic(const ThemeStatic& themeStatic);
	int AddStatic(float x, float y, float w, float h, 
		const RGB& colRGB, const path* textureFilename, 
		eTextureType textureType = TextureType::Plain, Uint32 color = 0xFFFFFFFF);
	int AddStatic(float x, float y, float w, float h, float z, 
		const RGB& colRGB, const path* textureFilename, 
		eTextureType textureType = TextureType::Plain, Uint32 color = 0xFFFFFFFF);
	int AddStatic(float x, float y, float w, float h, float z, 
		const RGB& colRGB, float texX1, float texY1, float texX2, float texY2,
		const path* textureFilename, 
		eTextureType textureType = TextureType::Plain,
		Uint32 color = 0xFFFFFFFF,
		bool reflection = false, float reflectionSpacing = 0.0f);

	// Text
	int AddText(const ThemeText& themeText);
	int AddText(float x, float y, const std::string& text);
	int AddText(float x, float y, Uint32 style, float size, 
		const RGB& colRGB, const std::string& text);
	int AddText(float x, float y, float w, Uint32 style, float size, 
		const RGB& colRGB, int align, const std::string& text,
		bool reflection, float reflectionSpacing, float z);

	// Button
	void SetButtonLength(size_t length);
	int AddButton(const ThemeButton& themeButton);
	int AddButton(float x, float y, float w, float h, 
		const path* texturePath,
		eTextureType textureType = TextureType::Plain,
		bool reflection = false);
	int AddButton(float x, float y, float w, float h, 
		const RGB& colRGB, float intensity,
		const RGB& dColRGB, float dIntensity,
		const path* texturePath,
		eTextureType textureType = TextureType::Plain,
		bool reflection = false,
		float reflectionSpacing = 0.0f,
		float deselectReflectionSpacing = 0.0f);
	void ClearButtons();
	void AddButtonText(float addX, float addY, const std::string& addText);
	void AddButtonText(float addX, float addY, const RGB& colRGB, 
		const std::string& addText);
	void AddButtonText(float addX, float addY, const RGB& colRGB, 
		int font, int size, int align, const std::string& addText);
	void AddButtonText(MenuButton& button, 
		float addX, float addY, const RGB& colRGB, 
		int font, int size, int align, const std::string& addText);

	// Select slide
	int AddSelectSlide(const ThemeSelectSlide& themeSelectSlide, Uint32 * pData, 
		const std::string* values, size_t valueCount);
	int AddSelectSlide(float x, float y, float w, float h, float skipX, float sbgw,
		const RGB& colRGB,     float intensity,
		const RGB& dColRGB,    float dIntensity,
		const RGB& tColRGB,    float tIntensity,
		const RGB& tdColRGB,   float tdIntensity,
		const RGB& sbgColRGB,  float sbgIntensity,
		const RGB& sbgdColRGB, float sbgdIntensity,
		const RGB& stColRGB,   float stIntensity,
		const RGB& stdColRGB,  float stdIntensity,
		const path* textureFilename,
		eTextureType textureType,
		const path* sbgFilename,
		eTextureType sbgTextureType,
		const std::string& caption, Uint32 * pData);
	void AddSelectSlideOption(const std::string& addText);
	void AddSelectSlideOption(Uint32 selectNum, const std::string& addText);
	void ResetSelectSlideOptions(Uint32 selectNo);
	void UpdateSelectSlideOptions(Uint32 selectNo, const std::vector<std::string>& values, int data);

	void FadeTo(Menu* screen);
	void FadeTo(Menu* screen, enum SoundType soundType);

	// Popup hack
	void CheckFadeTo(Menu* screen, const char * message);
	static void CheckFadeToCallback(bool value);

	virtual void DrawBG();
	virtual void DrawFG();
	virtual void Draw();
	virtual bool ParseInput(Uint32 pressedKey, SDL_Keycode keyCode, bool pressedDown);
	virtual bool ParseTextInput(SDL_Event * event);
	virtual bool ParseMouse(int mouseButton, bool btnDown, float x, float y);
	bool InRegion(float x, float y, const MouseOverRect& rect);
	int InteractAt(float x, float y);
	int CollectionAt(float x, float y);
	
	virtual void OnShow();
	virtual void OnShowFinish();
	virtual void OnHide();

	virtual void SetAnimationProgress(float progress);
	bool IsSelectable(Uint32 index);

	virtual bool InteractNext();
	virtual void InteractCustom(int customSwitch);
	virtual bool InteractPrev();
	virtual bool InteractInc();
	virtual bool InteractDec();
	virtual bool InteractNextRow(); // this is for the options screen, so button down makes sense
	virtual bool InteractPrevRow(); // this is for the options screen, so button up makes sense
	void AddBox(float x, float y, float w, float h);

	~Menu();

protected:
	MenuBackground	* Background;
	std::vector<MenuInteract> Interactions;
	int SelInteraction;

	int ButtonPos;
	std::vector<MenuButton> Buttons;

	std::vector<MenuSelectSlide> SelectSlides;
	std::vector<MenuButtonCollection> ButtonCollections;
};

#endif
