/*
"LettersFall" - Open-source cross-platform educational word spelling game.
Copyright (C) 2018  Jesse Palser

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
#include <stdio.h>
#include <cstring>
#include <iostream>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

#include "input.h"

#include "visuals.h"
#include "screens.h"
#include "audio.h"
#include "logic.h"

extern Visuals* visuals;
extern Screens* screens;
extern Audio* audio;
extern Logic* logic;

//-------------------------------------------------------------------------------------------------
Input::Input(void)
{
    DEBUG = false;

    EXIT_Game = false;

    DelayAllUserInput = 0;

    KeyOnKeyboardPressedByUser = -1;

    EnterKeyPressed = false;
    BackSpaceKeyPressed = false;
    SpacebarKeyPressed = false;
    EscapeKeyPressed = false;

    ArrowKeyFirstPressed = false;

    MouseButtonPressed[0] = false;
    MouseButtonPressed[1] = false;

    JoystickDeviceOne = NULL;
    JoystickDeviceTwo = NULL;
    JoystickDeviceThree = NULL;

    if (SDL_NumJoysticks() == 0)
    {
        printf("No USB joysticks are plugged in.\n");
    }
    else
    {
        if (SDL_NumJoysticks()>0)
        {
            JoystickDeviceOne = SDL_JoystickOpen(0);
            NumberOfJoyButtons[0] = SDL_JoystickNumButtons(JoystickDeviceOne);
            NumberOfJoyAxises[0] = SDL_JoystickNumAxes(JoystickDeviceOne);
            printf("SDL2 Joystick 0 initialized.\n");
        }

        if (SDL_NumJoysticks()>1)
        {
            JoystickDeviceTwo = SDL_JoystickOpen(1);
            NumberOfJoyButtons[1] = SDL_JoystickNumButtons(JoystickDeviceTwo);
            NumberOfJoyAxises[1] = SDL_JoystickNumAxes(JoystickDeviceTwo);
            printf("SDL2 Joystick 1 initialized.\n");
        }

        if (SDL_NumJoysticks()>2)
        {
            JoystickDeviceThree = SDL_JoystickOpen(2);
            NumberOfJoyButtons[2] = SDL_JoystickNumButtons(JoystickDeviceThree);
            NumberOfJoyAxises[2] = SDL_JoystickNumAxes(JoystickDeviceThree);
            printf("SDL2 Joystick 2 initialized.\n");
        }
    }

    for (int joy = 0; joy < 3; joy++)
    {
        JoyUP[joy] = Axis1;
        JoyDOWN[joy] = Axis1;
        JoyLEFT[joy] = Axis0;
        JoyRIGHT[joy] = Axis0;
        JoyButton1[joy] = Button0;
        JoyButton2[joy] = Button1;
    }

    JoystickSetupProcess = JoySetupNotStarted;

    for (int index = 0; index < 5; index++)
    {
        JoystickDirectionHorizonal[index] = CENTER;
        JoystickDirectionVertical[index] = CENTER;
        JoystickButtonOne[index] = OFF;
        JoystickButtonTwo[index] = OFF;
        JoystickButton1Pressed[index] = false;
        JoystickButton2Pressed[index] = false;
    }

    KeyboardSetupProcess = KeyboardSetupNotStarted;
    UserDefinedKeyButtonOne = -1;
    UserDefinedKeyButtonTwo = -1;
    UserDefinedKeyUP = -1;
    UserDefinedKeyRIGHT = -1;
    UserDefinedKeyDOWN = -1;
    UserDefinedKeyLEFT = -1;
    UserDefinedKeyPause = -1;

    DelayAllUserInput = -1;
}

//-------------------------------------------------------------------------------------------------
Input::~Input(void)
{
    if (JoystickDeviceOne != NULL)  SDL_JoystickClose(JoystickDeviceOne);
    if (JoystickDeviceTwo != NULL)  SDL_JoystickClose(JoystickDeviceTwo);
    if (JoystickDeviceThree != NULL)  SDL_JoystickClose(JoystickDeviceThree);
}

//-------------------------------------------------------------------------------------------------
int Input::QueryJoysticksForAction(Uint8 joy)
{
int returnValue = -1;

	if (DelayAllUserInput > 0)  return(-1);

	if (joy == 0 && JoystickDeviceOne != NULL)
	{
		SDL_JoystickUpdate();

        for (int index = 0; index < NumberOfJoyAxises[joy]; index++)
        {
            Sint16 joyAxis = 0;
            joyAxis = SDL_JoystickGetAxis(JoystickDeviceOne, index);
            if (joyAxis < -16383 || joyAxis > 16383)
            {
                returnValue = Axis0+index;
                return(returnValue);
            }
        }

        for (int index = 0; index < NumberOfJoyButtons[joy]; index++)
        {
            if ( SDL_JoystickGetButton(JoystickDeviceOne, index) )
            {
                returnValue = Button0+index;
                return(returnValue);
            }
        }
    }

	if (joy == 1 && JoystickDeviceTwo != NULL)
	{
		SDL_JoystickUpdate();

        for (int index = 0; index < NumberOfJoyAxises[joy]; index++)
        {
            Sint16 joyAxis = 0;
            joyAxis = SDL_JoystickGetAxis(JoystickDeviceTwo, index);
            if (joyAxis < -16383 || joyAxis > 16383)
            {
                returnValue = Axis0+index;
                return(returnValue);
            }
        }

        for (int index = 0; index < NumberOfJoyButtons[joy]; index++)
        {
            if ( SDL_JoystickGetButton(JoystickDeviceTwo, index) )
            {
                returnValue = Button0+index;
                return(returnValue);
            }
        }
    }

	if (joy == 2 && JoystickDeviceThree != NULL)
	{
		SDL_JoystickUpdate();

        for (int index = 0; index < NumberOfJoyAxises[joy]; index++)
        {
            Sint16 joyAxis = 0;
            joyAxis = SDL_JoystickGetAxis(JoystickDeviceThree, index);
            if (joyAxis < -16383 || joyAxis > 16383)
            {
                returnValue = Axis0+index;
                return(returnValue);
            }
        }

        for (int index = 0; index < NumberOfJoyButtons[joy]; index++)
        {
            if ( SDL_JoystickGetButton(JoystickDeviceThree, index) )
            {
                returnValue = Button0+index;
                return(returnValue);
            }
        }
    }

    return(returnValue);
}

//-------------------------------------------------------------------------------------------------
void Input::GetAllUserInput(void)
{
    if (EscapeKeyPressed == true)
    {
        if (screens->ScreenToDisplay == TitleScreen)  EXIT_Game = true;
        else  screens->ScreenTransitionStatus = FadeOut;
    }

    KeyOnKeyboardPressedByUser = -1;

    EnterKeyPressed = false;
    BackSpaceKeyPressed = false;
    SpacebarKeyPressed = false;
    EscapeKeyPressed = false;

    MouseButtonPressed[0] = false;
    MouseButtonPressed[1] = false;

    for (int index = 0; index < 5; index++)
    {
        JoystickDirectionHorizonal[index] = CENTER;
        JoystickDirectionVertical[index] = CENTER;
        JoystickButtonOne[index] = OFF;
        JoystickButtonTwo[index] = OFF;
    }

    if (DelayAllUserInput > 0)
    {
        DelayAllUserInput--;
        return;
    }

    KeyModState = SDL_GetModState();

    if (KeyModState & KMOD_LSHIFT)  ShiftKeyPressed = true;
    else if (KeyModState & KMOD_RSHIFT)  ShiftKeyPressed = true;
    else  ShiftKeyPressed = false;

    while ( SDL_PollEvent(&Event) )
    {
        switch (Event.type)
        {
            case SDL_QUIT:
                EXIT_Game = true;
                return;
                break;

            case SDL_WINDOWEVENT:
                if (Event.window.event == SDL_WINDOWEVENT_RESIZED || Event.window.event == SDL_WINDOWEVENT_SHOWN)
                {
                    screens->ScreenIsDirty = true;
                    visuals->ClearTextCache();
                }
                break;

            case SDL_KEYDOWN:
                {
                    char keyDown = Event.key.keysym.scancode;

                    if (keyDown == SDL_SCANCODE_RETURN)  EnterKeyPressed = true;
                    else if (keyDown == SDL_SCANCODE_BACKSPACE)  BackSpaceKeyPressed = true;
                    else if (keyDown == SDL_SCANCODE_SPACE)  SpacebarKeyPressed = true;
                    else if (keyDown == SDL_SCANCODE_ESCAPE)  EscapeKeyPressed = true;
                }
                break;

            case SDL_TEXTINPUT:
                for(int i=0; i<SDL_TEXTINPUTEVENT_TEXT_SIZE; ++i)
                {
                    char c = Event.text.text[0];
                    if(c < ' ' || c > '~')  break;

                    KeyOnKeyboardPressedByUser = (char)c;
                }
                break;


            default:
                break;
        }
    }

    bool usingCustomKeys = true;
    if (UserDefinedKeyButtonOne == -1
     || UserDefinedKeyButtonTwo == -1
     || UserDefinedKeyUP == -1
     || UserDefinedKeyRIGHT == -1
     || UserDefinedKeyDOWN == -1
     || UserDefinedKeyLEFT == -1
     || UserDefinedKeyPause == -1)  usingCustomKeys = false;

    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
    if ( (screens->ScreenToDisplay != PlayingGameScreen)
     || (screens->ScreenToDisplay == PlayingGameScreen && usingCustomKeys == false) )
    {
		if (keyboardState[SDL_SCANCODE_Z] && screens->ScreenToDisplay != NameInputMouseKeyboardScreen)  JoystickButtonOne[Keyboard] = ON;
		else if (keyboardState[SDL_SCANCODE_X] && screens->ScreenToDisplay != NameInputMouseKeyboardScreen)  JoystickButtonTwo[Keyboard] = ON;

        if      (keyboardState[   SDL_SCANCODE_UP])
        {
            if (ArrowKeyFirstPressed == true)  JoystickDirectionVertical[Keyboard] = UP;
            else  ArrowKeyFirstPressed = true;
        }
        else if (keyboardState[ SDL_SCANCODE_DOWN])
        {
            if (ArrowKeyFirstPressed == true)  JoystickDirectionVertical[Keyboard] = DOWN;
            else  ArrowKeyFirstPressed = true;
        }

        if      (keyboardState[ SDL_SCANCODE_LEFT])
        {
            if (ArrowKeyFirstPressed == true)  JoystickDirectionHorizonal[Keyboard] = LEFT;
            else  ArrowKeyFirstPressed = true;
        }
        else if (keyboardState[SDL_SCANCODE_RIGHT])
        {
            if (ArrowKeyFirstPressed == true)  JoystickDirectionHorizonal[Keyboard] = RIGHT;
            else  ArrowKeyFirstPressed = true;
        }
    }
    else if (screens->ScreenToDisplay == PlayingGameScreen && usingCustomKeys == true)
    {
		if (keyboardState[UserDefinedKeyButtonOne] && screens->ScreenToDisplay != NameInputMouseKeyboardScreen)  JoystickButtonOne[Keyboard] = ON;
		else if (keyboardState[UserDefinedKeyButtonTwo] && screens->ScreenToDisplay != NameInputMouseKeyboardScreen)  JoystickButtonTwo[Keyboard] = ON;

        if      (keyboardState[       UserDefinedKeyUP])  JoystickDirectionVertical[Keyboard] = UP;
        else if (keyboardState[     UserDefinedKeyDOWN])  JoystickDirectionVertical[Keyboard] = DOWN;

        if      (keyboardState[     UserDefinedKeyLEFT])  JoystickDirectionHorizonal[Keyboard] = LEFT;
        else if (keyboardState[    UserDefinedKeyRIGHT])  JoystickDirectionHorizonal[Keyboard] = RIGHT;
    }

//------------------------------------------------------------------------
    SDL_PumpEvents();
    SDL_GetMouseState(&MouseX, &MouseY);

    float tempXtwo;
    float tempYtwo;
    if (visuals->WindowWidthCurrent != 640)
    {
        tempXtwo = (float)visuals->WindowWidthCurrent / 640;
        MouseX = (int)(MouseX / tempXtwo);
    }

    if (visuals->WindowHeightCurrent != 480)
    {
		tempYtwo = (float)visuals->WindowHeightCurrent / 480;
        MouseY = (int)(MouseY / tempYtwo);
    }

    for (int index = 0; index < 2; index++)
    {
        if ( SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(index+1) )
        {
            if (MouseButtonWasClicked[index] == false)
            {
                MouseButtonPressed[index] = true;
                MouseButtonWasClicked[index] = true;
            }
        }
        else  MouseButtonWasClicked[index] = false;
    }

//------------------------------------------------------------------------
/*    if (JoystickDeviceOne != NULL)
	{
		SDL_JoystickUpdate();

		Sint16 joystickXmovement = 0;
        if (JoyLEFT[0] > Axis7)
        {
            if ( SDL_JoystickGetButton(JoystickDeviceOne, JoyLEFT[0]-8) )  JoystickDirectionHorizonal[JoystickOne] = LEFT;
            else  if ( SDL_JoystickGetButton(JoystickDeviceOne, JoyRIGHT[0]-8) )  JoystickDirectionHorizonal[JoystickOne] = RIGHT;
        }
        else joystickXmovement = SDL_JoystickGetAxis(JoystickDeviceOne, JoyLEFT[0]);

		Sint16 joystickYmovement = 0;
        if (JoyUP[0] > Axis7)
        {
            if ( SDL_JoystickGetButton(JoystickDeviceOne, JoyUP[0]-8) )  JoystickDirectionVertical[JoystickOne] = UP;
            else  if ( SDL_JoystickGetButton(JoystickDeviceOne, JoyDOWN[0]-8) )  JoystickDirectionVertical[JoystickOne] = DOWN;
        }
        else joystickYmovement = SDL_JoystickGetAxis(JoystickDeviceOne, JoyUP[0]);

		if (joystickYmovement < -16383)
		{
            JoystickDirectionVertical[JoystickOne] = UP;
		}
		else if (joystickYmovement > 16383)
		{
            JoystickDirectionVertical[JoystickOne] = DOWN;
		}

		if (joystickXmovement < -16383)
		{
            JoystickDirectionHorizonal[JoystickOne] = LEFT;
		}
		else if (joystickXmovement > 16383)
		{
            JoystickDirectionHorizonal[JoystickOne] = RIGHT;
		}

        if (JoyButton1[0] > Axis7)
        {
	    	if ( SDL_JoystickGetButton(JoystickDeviceOne, JoyButton1[0]-8) )  JoystickButtonOne[JoystickOne] = ON;
        }
        else
        {
            Sint16 padAsButton = SDL_JoystickGetAxis(JoystickDeviceOne, JoyButton1[0]);
            if (padAsButton < -16383)  JoystickButtonOne[JoystickOne] = ON;
        }

        if (JoyButton2[0] > Axis7)
        {
    		if ( SDL_JoystickGetButton(JoystickDeviceOne, JoyButton2[0]-8) )  JoystickButtonTwo[JoystickOne] = ON;
        }
        else
        {
            Sint16 padAsButton = SDL_JoystickGetAxis(JoystickDeviceOne, JoyButton2[0]);
            if (padAsButton < -16383)  JoystickButtonTwo[JoystickOne] = ON;
        }
    }

    if (JoystickDeviceTwo != NULL)
	{
		SDL_JoystickUpdate();

		Sint16 joystickXmovement = 0;
        if (JoyLEFT[1] > Axis7)
        {
            if ( SDL_JoystickGetButton(JoystickDeviceTwo, JoyLEFT[1]-8) )  JoystickDirectionHorizonal[JoystickTwo] = LEFT;
            else  if ( SDL_JoystickGetButton(JoystickDeviceTwo, JoyRIGHT[1]-8) )  JoystickDirectionHorizonal[JoystickTwo] = RIGHT;
        }
        else joystickXmovement = SDL_JoystickGetAxis(JoystickDeviceTwo, JoyLEFT[1]);

		Sint16 joystickYmovement = 0;
        if (JoyUP[1] > Axis7)
        {
            if ( SDL_JoystickGetButton(JoystickDeviceTwo, JoyUP[1]-8) )  JoystickDirectionVertical[JoystickTwo] = UP;
            else  if ( SDL_JoystickGetButton(JoystickDeviceTwo, JoyDOWN[1]-8) )  JoystickDirectionVertical[JoystickTwo] = DOWN;
        }
        else joystickYmovement = SDL_JoystickGetAxis(JoystickDeviceTwo, JoyUP[1]);

		if (joystickYmovement < -16383)
		{
            JoystickDirectionVertical[JoystickTwo] = UP;
		}
		else if (joystickYmovement > 16383)
		{
            JoystickDirectionVertical[JoystickTwo] = DOWN;
		}

		if (joystickXmovement < -16383)
		{
            JoystickDirectionHorizonal[JoystickTwo] = LEFT;
		}
		else if (joystickXmovement > 16383)
		{
            JoystickDirectionHorizonal[JoystickTwo] = RIGHT;
		}

        if (JoyButton1[1] > Axis7)
        {
	    	if ( SDL_JoystickGetButton(JoystickDeviceTwo, JoyButton1[1]-8) )  JoystickButtonOne[JoystickTwo] = ON;
        }
        else
        {
            Sint16 padAsButton = SDL_JoystickGetAxis(JoystickDeviceTwo, JoyButton1[1]);
            if (padAsButton < -16383)  JoystickButtonOne[JoystickTwo] = ON;
        }

        if (JoyButton2[1] > Axis7)
        {
    		if ( SDL_JoystickGetButton(JoystickDeviceTwo, JoyButton2[1]-8) )  JoystickButtonTwo[JoystickTwo] = ON;
        }
        else
        {
            Sint16 padAsButton = SDL_JoystickGetAxis(JoystickDeviceTwo, JoyButton2[1]);
            if (padAsButton < -16383)  JoystickButtonTwo[JoystickTwo] = ON;
        }
    }

    if (JoystickDeviceThree != NULL)
	{
		SDL_JoystickUpdate();

		Sint16 joystickXmovement = 0;
        if (JoyLEFT[2] > Axis7)
        {
            if ( SDL_JoystickGetButton(JoystickDeviceThree, JoyLEFT[2]-8) )  JoystickDirectionHorizonal[JoystickThree] = LEFT;
            else  if ( SDL_JoystickGetButton(JoystickDeviceThree, JoyRIGHT[2]-8) )  JoystickDirectionHorizonal[JoystickThree] = RIGHT;
        }
        else joystickXmovement = SDL_JoystickGetAxis(JoystickDeviceThree, JoyLEFT[2]);

		Sint16 joystickYmovement = 0;
        if (JoyUP[2] > Axis7)
        {
            if ( SDL_JoystickGetButton(JoystickDeviceThree, JoyUP[2]-8) )  JoystickDirectionVertical[JoystickThree] = UP;
            else  if ( SDL_JoystickGetButton(JoystickDeviceThree, JoyDOWN[2]-8) )  JoystickDirectionVertical[JoystickThree] = DOWN;
        }
        else joystickYmovement = SDL_JoystickGetAxis(JoystickDeviceThree, JoyUP[2]);

		if (joystickYmovement < -16383)
		{
            JoystickDirectionVertical[JoystickThree] = UP;
		}
		else if (joystickYmovement > 16383)
		{
            JoystickDirectionVertical[JoystickThree] = DOWN;
		}

		if (joystickXmovement < -16383)
		{
            JoystickDirectionHorizonal[JoystickThree] = LEFT;
		}
		else if (joystickXmovement > 16383)
		{
            JoystickDirectionHorizonal[JoystickThree] = RIGHT;
		}

        if (JoyButton1[2] > Axis7)
        {
	    	if ( SDL_JoystickGetButton(JoystickDeviceThree, JoyButton1[2]-8) )  JoystickButtonOne[JoystickThree] = ON;
        }
        else
        {
            Sint16 padAsButton = SDL_JoystickGetAxis(JoystickDeviceThree, JoyButton1[2]);
            if (padAsButton < -16383)  JoystickButtonOne[JoystickThree] = ON;
        }

        if (JoyButton2[2] > Axis7)
        {
    		if ( SDL_JoystickGetButton(JoystickDeviceThree, JoyButton2[2]-8) )  JoystickButtonTwo[JoystickThree] = ON;
        }
        else
        {
            Sint16 padAsButton = SDL_JoystickGetAxis(JoystickDeviceThree, JoyButton2[2]);
            if (padAsButton < -16383)  JoystickButtonTwo[JoystickThree] = ON;
        }
    }*/
//------------------------------------------------------------------------

    JoystickDirectionHorizonal[Any] = CENTER;
    JoystickDirectionVertical[Any] = CENTER;
    JoystickButtonOne[Any] = OFF;
    JoystickButtonTwo[Any] = OFF;

    for (int index = 0; index < 4; index++)
    {
        if (JoystickDirectionHorizonal[Any] == CENTER)  JoystickDirectionHorizonal[Any] = JoystickDirectionHorizonal[index];
        if (JoystickDirectionVertical[Any] == CENTER)  JoystickDirectionVertical[Any] = JoystickDirectionVertical[index];
        if (JoystickButtonOne[Any] == OFF)  JoystickButtonOne[Any] = JoystickButtonOne[index];
        if (JoystickButtonTwo[Any] == OFF)  JoystickButtonTwo[Any] = JoystickButtonTwo[index];
    }

//------------------------------------------------------------------------
}
