/*
"LettersFall" - Open-source cross-platform educational word spelling game.
Copyright (C) 2019  Jesse Palser

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

Email the author at: "JessePalser@Gmail.com"
*/
#ifndef INTERFACE
#define INTERFACE

class Interface
{
public:

	Interface(void);
	virtual ~Interface(void);

    #define NumberOfButtons     7
    struct ButtonGUI
    {
        Uint16 SpriteIndex;
        Sint8 ScreenIndex;
        float Scale;
        int ScreenY;
        Sint16 AnimationTimer;
    } ButtonGUIs[NumberOfButtons];

    Uint8 ButtonSelectedByKeyboard;
    Uint8 ButtonSelectedByPlayer;

    #define NumberOfArrowSets     10
    struct ArrowSetGUI
    {
        float LeftArrowScreenIndex;
        float LeftArrowScale;
        float RightArrowScale;
        int ScreenY;
        Sint16 AnimationTimer;
    } ArrowSetGUIs[NumberOfArrowSets];

    Uint8 ArrowSetSelectedByKeyboard;
    float ArrowSetArrowSelectedByPlayer;

    #define NumberOfIcons       10
    struct IconGUI
    {
        int Index;
        int SpriteIndex;
        int ScreenX;
        int ScreenY;
        Sint16 AnimationTimer;
        float Scale;
    } IconGUIs[NumberOfIcons];

    Uint8 IconSelectedByPlayer;

    void CreateButton(Uint16 SpriteIndex, Uint8 ScreenIndex, int ScreenY);
    void DisplayAllButtonsOntoScreenBuffer(void);
    void ProcessAllButtons(void);
    void DestroyAllButtons(void);

    void CreateArrowSet(float ScreenIndex, int ScreenY);
    void DisplayAllArrowSetsOntoScreenBuffer(void);
    void ProcessAllArrowSets(void);
    void DestroyAllArrowSets(void);

    void CreateIcon(int spriteIndex, int screenX, int screenY);
    void DisplayAllIconsOntoScreenBuffer(void);
    void ProcessAllIcons(void);
    void DestroyAllIcons(void);

};

#endif
