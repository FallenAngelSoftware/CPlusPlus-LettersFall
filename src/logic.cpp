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
using namespace std;
#include <fstream>
#include <string.h>

#include <sstream>
#include <iostream>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

#include "logic.h"

#include "audio.h"
#include "screens.h"
#include "input.h"
#include "visuals.h"

extern Audio* audio;
extern Screens* screens;
extern Input* input;
extern Visuals* visuals;

//-------------------------------------------------------------------------------------------------
Logic::Logic(void)
{
    RunLoop = false;

    GameMode = TeenagerMode;

    SelectedBackground = 0;
    SelectedMusicTrack = 0;

    MusicJukebox = false;

    InitialGameSpeed[0] = 45;
    InitialGameSpeed[1] = 30;
    InitialGameSpeed[2] = 16;
    InitialGameSpeed[3] = 30;
    InitialGameSpeed[4] = 30;

    Shareware = false;

    PAUSEgame = false;
}

//-------------------------------------------------------------------------------------------------
Logic::~Logic(void)
{

}

//-------------------------------------------------------------------------------------------------
Uint8 Logic::GetRandomLetter(void)
{
Uint8 returnValue = 1;
Uint32 random = rand()%NumberOfLetters;

    returnValue = (Uint8)random + 1;
	return(returnValue);
}

//-------------------------------------------------------------------------------------------------
void Logic::SetupNextLetters(void)
{
    DrawEntirePlayingGameScreen = 2;

    if (Level < 10)
    {
        FallingLetters[0] = NextLetters[0];
        FallingLetters[1] = NextLetters[1];
        FallingLetters[2] = NextLetters[2];

        NextLetters[0] = GetRandomLetter();
        NextLetters[1] = 0;
        NextLetters[2] = 0;
    }
    else if (Level < 20)
    {
        FallingLetters[0] = NextLetters[0];
        FallingLetters[1] = NextLetters[1];
        FallingLetters[2] = NextLetters[2];

        NextLetters[0] = GetRandomLetter();
        NextLetters[1] = GetRandomLetter();
        NextLetters[2] = 0;
    }
    else if (Level < 30)
    {
        FallingLetters[0] = NextLetters[0];
        FallingLetters[1] = NextLetters[1];
        FallingLetters[2] = NextLetters[2];

        NextLetters[0] = GetRandomLetter();
        NextLetters[1] = GetRandomLetter();
        NextLetters[2] = GetRandomLetter();
    }

    FallingLettersPlayfieldX[0] = FallingLettersNextPlayfieldX;
    FallingLettersPlayfieldY[0] = 2;
    FallingLettersScreenX[0] = 27 + (FallingLettersNextPlayfieldX * 41);
    FallingLettersScreenY[0] = -4;

    FallingLettersPlayfieldX[1] = FallingLettersNextPlayfieldX;
    FallingLettersPlayfieldY[1] = 1;
    FallingLettersScreenX[1] = 27 + (FallingLettersNextPlayfieldX * 41);
    FallingLettersScreenY[1] = -4 - (1*30);

    FallingLettersPlayfieldX[2] = FallingLettersNextPlayfieldX;
    FallingLettersPlayfieldY[2] = 0;
    FallingLettersScreenX[2] = 27 + (FallingLettersNextPlayfieldX * 41);
    FallingLettersScreenY[2] = -4 - (2*30);

    FallingLettersOffsetY = 0;

    ClearWordTransparency = 255;

    if (FallingLettersNextPlayfieldX < 10)  FallingLettersNextPlayfieldX++;
    else if (FallingLettersNextPlayfieldX == 10)  FallingLettersNextPlayfieldX = 0;

    if (Playfield[ FallingLettersPlayfieldX[0] ][ FallingLettersPlayfieldY[0]+1 ] > 0)
    {
        GameOver = 200;
    }
}

//-------------------------------------------------------------------------------------------------
void Logic::SetupForNewGame(void)
{
    DrawEntirePlayingGameScreen = 2;

    GameOver = -1;

    CurrentGameSpeed = InitialGameSpeed[GameMode];
    visuals->FrameLock = CurrentGameSpeed;

    if (input->DEBUG == false)
    {
        Score = 0;
        Level = 0;
        Bombs = 1;

        BombBonusMeterValue = 0;
    }
    else
    {
        Score = 45793;
        Level = 13;
        Bombs = 5;

        BombBonusMeterValue = 75;
    }

    for (int y = 0; y < 17; y++)
    {
        for (int x = 0; x < 11; x++)
        {
            Playfield[x][y] = 0;
        }
    }

    for (int y = 12; y < 17; y++)
    {
        for (int x = 0; x < 11; x++)
        {
            Playfield[x][y] = GetRandomLetter();
        }
    }

    for (int x = 0; x < 11; x++)  Playfield[x][17] = 255;

    NextLetters[0] = GetRandomLetter();
    NextLetters[1] = 0;
    NextLetters[2] = 0;

    FallingLettersNextPlayfieldX = 0;
    SetupNextLetters();

    DownArrowScale = 1;
    DownArrowPressedTimer = -1;
    LeftArrowScale = 1;
    LeftArrowPressedTimer = -1;
    RightArrowScale = 1;
    RightArrowPressedTimer = -1;

    DroppingFast = false;

    BombButtonPressedTimer = -1;
    BombButtonScale = 1;
    ExplosionScale = -1;
    ExplosionTransparency = 255;

    for (int index = 0; index < 11; index++)
    {
        SelectedLetterPlayfieldX[index] = -1;
        SelectedLetterPlayfieldY[index] = -1;
    }

    ClearButtonScale = 1;
    ClearButtonPressedTimer = -1;
    UndoButtonScale = 1;
    UndoButtonPressedTimer = -1;

    CheckButtonScale = 1;
    CheckButtonPressedTimer = -1;
    CheckButtonRedHue = 255;
    CheckButtonGreenHue = 255;
    CheckButtonBlueHue = 255;

    ClearWordTransparency = 255;

    LevelAdvancementValue = 0;

    if (GameMode == TimeAttack15MinuteMode)
    {
        TimeAttackMinutes = 14;
        TimeAttackSeconds = 59;
    }
    else
    {
        TimeAttackMinutes = -1;
        TimeAttackSeconds = -1;
    }

    TimeAttackNextSecondTick = SDL_GetTicks()+1000;

    Playfield[0][16] = 10; // J
    Playfield[1][13] = 15; // O
    Playfield[2][14] = 25; // Y
    Playfield[3][16] = 6;  // F
    Playfield[4][13] = 21; // U
    Playfield[5][16] = 12; // L
    Playfield[6][14] = 12; // L
    Playfield[7][15] = 25; // Y
}

//-------------------------------------------------------------------------------------------------
void Logic::DeleteLettersInPlayfieldFromExplosion(void)
{
    DrawEntirePlayingGameScreen = 2;

    ClearSelectedLetters();

    for (int y = -2; y < 3; y++)
    {
        for (int x = -2; x < 3; x++)
        {
            if (  ( (FallingLettersPlayfieldY[0]+y) > -1 ) && ( (FallingLettersPlayfieldY[0]+y) < 17 )
                 && ( (FallingLettersPlayfieldX[0]+x) > -1 ) && ( (FallingLettersPlayfieldX[0]+x) < 11 )  )
                Playfield[ FallingLettersPlayfieldX[0]+x ][ FallingLettersPlayfieldY[0]+y ] = 0;
        }
    }

    for (int x = -1; x < 2; x++)
    {
        if (  ( (FallingLettersPlayfieldY[0]-3) > -1 ) && ( (FallingLettersPlayfieldY[0]-3) < 17 )
             && ( (FallingLettersPlayfieldX[0]+x) > -1 ) && ( (FallingLettersPlayfieldX[0]+x) < 11 )  )
            Playfield[ FallingLettersPlayfieldX[0]+x ][ FallingLettersPlayfieldY[0]-3 ] = 0;
    }

    for (int x = -1; x < 2; x++)
    {
        if (  ( (FallingLettersPlayfieldY[0]+3) > -1 ) && ( (FallingLettersPlayfieldY[0]+3) < 17 )
             && ( (FallingLettersPlayfieldX[0]+x) > -1 ) && ( (FallingLettersPlayfieldX[0]+x) < 11 )  )
            Playfield[ FallingLettersPlayfieldX[0]+x ][ FallingLettersPlayfieldY[0]+3 ] = 0;
    }

    for (int y = -1; y < 2; y++)
    {
        if (  ( (FallingLettersPlayfieldY[0]+y) > -1 ) && ( (FallingLettersPlayfieldY[0]+y) < 17 )
             && ( (FallingLettersPlayfieldX[0]-3) > -1 ) && ( (FallingLettersPlayfieldX[0]-3) < 11 )  )
            Playfield[ FallingLettersPlayfieldX[0]-3 ][ FallingLettersPlayfieldY[0]+y ] = 0;
    }

    for (int y = -1; y < 2; y++)
    {
        if (  ( (FallingLettersPlayfieldY[0]+y) > -1 ) && ( (FallingLettersPlayfieldY[0]+y) < 17 )
             && ( (FallingLettersPlayfieldX[0]+3) > -1 ) && ( (FallingLettersPlayfieldX[0]+3) < 11 )  )
            Playfield[ FallingLettersPlayfieldX[0]+3 ][ FallingLettersPlayfieldY[0]+y ] = 0;
    }
}

//-------------------------------------------------------------------------------------------------
bool Logic::ApplyGravityToPlayfield(void)
{
bool returnValue = false;

    for (int y = 16; y > 0; y--)
    {
        for (int x = 0; x < 11; x++)
        {
            if (Playfield[x][y] == 0 && Playfield[x][y-1] > 0)
            {
                Playfield[x][y] = Playfield[x][y-1];
                Playfield[x][y-1] = 0;

                DrawEntirePlayingGameScreen = 2;

                returnValue = true;
            }
        }
    }

    return(returnValue);
}

//-------------------------------------------------------------------------------------------------
void Logic::GetDataFromFile(const char* fileName, std::stringstream& ss)
{
    std::ifstream file(fileName);
    if(file.is_open())
    {
        ss << file.rdbuf();
    }
    file.close();
}

//-------------------------------------------------------------------------------------------------
bool Logic::CheckSelectedLettersForWord(void)
{
char filename[256];
char *base_path = SDL_GetBasePath();
std::string letterASCII;
Uint16 numberOfSelectedLetters = 0;
std::string userWord;
bool returnValue = false;

    if (SelectedLetterPlayfieldX[2] == -1 && SelectedLetterPlayfieldY[2] == -1)  return(returnValue);

    int letter = 0;
    while (letter < 11 && SelectedLetterPlayfieldX[letter] != -1 && SelectedLetterPlayfieldY[letter] != -1)
    {
        numberOfSelectedLetters++;

        letterASCII = (char)(Playfield[ SelectedLetterPlayfieldX[letter] ][ SelectedLetterPlayfieldY[letter] ] + 96);
        userWord += letterASCII;

        letter++;
    }

	strcpy(filename, base_path);
	strcat(filename, "data/dictionary/");
	int firstLetterASCII = ('A' + Playfield[ SelectedLetterPlayfieldX[0] ][ SelectedLetterPlayfieldY[0] ] - 1);
	char firstLetter[2];
	sprintf(firstLetter, "%c", firstLetterASCII);
    strcat( filename, firstLetter );
    strcat(filename, "-Words.txt");

    std::stringstream ss;
    GetDataFromFile(filename, ss);

    std::string data;
    while ( std::getline(ss, data) )
    {
        int numberOfLettersInDictionary = 0;
        for (int index = 0; index < 50; index++)
        {
            if (data[index] >= 'a' && data[index] <= 'z')  numberOfLettersInDictionary++;
            else  index = 51;
        }

        if (numberOfSelectedLetters == numberOfLettersInDictionary)//data.length() )
        {
            for (int index = 0; index < numberOfSelectedLetters; index++)
            {
                if (userWord[index] == data[index])  returnValue = true;
                else
                {
                    returnValue = false;
                    break;
                }
            }
        }

        if (returnValue == true)  break;
    }

    return(returnValue);
}

//-------------------------------------------------------------------------------------------------
void Logic::ClearSelectedLetters(void)
{
    for (int index = 0; index < 11; index++)
    {
        SelectedLetterPlayfieldX[index] = -1;
        SelectedLetterPlayfieldY[index] = -1;
    }
}

//-------------------------------------------------------------------------------------------------
void Logic::ClearSelectedLettersInPlayfield(void)
{
    int wordLength = 0;
    for (int index = 0; index < 11; index++)
    {
        if (SelectedLetterPlayfieldX[index] != -1 && SelectedLetterPlayfieldY[index] != -1)  wordLength++;
    }

    Score+=( 25*wordLength*(Level+1) );

    if (wordLength > 7)
    {
        audio->PlayDigitalSoundFX(5, 0);
        Score+=( 1000 * (Level+1) );

        audio->PlayDigitalSoundFX(9, 0);
    }

    BombBonusMeterValue+=( (wordLength-2)*4 );
    if (BombBonusMeterValue > 147)
    {
        BombBonusMeterValue = 0;
        Score+=1000*Level;
        Bombs++;
        audio->PlayDigitalSoundFX(8, 0);
    }

    LevelAdvancementValue++;
    if (LevelAdvancementValue > 6)
    {
        Level++;
        LevelAdvancementValue = 0;

        if (Level % 10 == 0)
        {
            CurrentGameSpeed = InitialGameSpeed[GameMode];
            visuals->FrameLock = CurrentGameSpeed;
        }
        else
        {
            CurrentGameSpeed--;
            visuals->FrameLock = CurrentGameSpeed;
        }

        audio->PlayDigitalSoundFX(7, 0);
    }

    for (int index = 0; index < 11; index++)
    {
        if (SelectedLetterPlayfieldX[index] != -1 && SelectedLetterPlayfieldY[index] != -1)
            Playfield[ SelectedLetterPlayfieldX[index] ][ SelectedLetterPlayfieldY[index] ] = 0;
    }
}

//-------------------------------------------------------------------------------------------------
void Logic::RunGameplayEngine(void)
{
float movementY = 0;
Uint32 ticks = SDL_GetTicks();

    if (FallingLetters[0] == 255)  movementY = 1;
    else  movementY = 5;

    if (GameMode == TimeAttack15MinuteMode)
    {
        if (ticks >= TimeAttackNextSecondTick)
        {
            if (TimeAttackSeconds > 0)
            {
                TimeAttackSeconds--;
            }
            else if (TimeAttackMinutes > 0)
            {
                TimeAttackMinutes--;
                TimeAttackSeconds = 59;
            }
            else
            {
                TimeAttackMinutes = 0;
                TimeAttackSeconds = 0;

                GameOver = 150;
            }

            screens->ScreenIsDirty = true;
            DrawEntirePlayingGameScreen = 2;
            TimeAttackNextSecondTick = (ticks+1000);
        }
    }

    if (LeftArrowScale == 0.75f)
    {
        if (LeftArrowPressedTimer > -1)  LeftArrowPressedTimer--;
        else
        {
            LeftArrowScale = 1;
            DrawEntirePlayingGameScreen = 2;
        }
    }

    if (RightArrowScale == 0.75f)
    {
        if (RightArrowPressedTimer > -1)  RightArrowPressedTimer--;
        else
        {
            RightArrowScale = 1;
            DrawEntirePlayingGameScreen = 2;
        }
    }

    if (BombButtonPressedTimer == 10)
    {
        DrawEntirePlayingGameScreen = 2;

        BombButtonPressedTimer--;
    }
    else if (BombButtonPressedTimer > 5)
    {
        DrawEntirePlayingGameScreen = 2;

        BombButtonScale = 0.75;

        BombButtonPressedTimer--;
    }
    else if (BombButtonPressedTimer > 0)
    {
        BombButtonScale = 1;

        BombButtonPressedTimer--;

        DrawEntirePlayingGameScreen = 2;
    }
    else if (BombButtonPressedTimer == 0)
    {
        DrawEntirePlayingGameScreen = 2;

        BombButtonPressedTimer = -1;

        Bombs--;

        ExplosionTransparency = 255;

        NextLetters[0] = 255;
        NextLetters[1] = 0;
        NextLetters[2] = 0;
    }

    if (ClearButtonPressedTimer > -1)  ClearButtonPressedTimer--;

    if (ClearButtonScale == 0.75)
    {
        if (ClearButtonPressedTimer == 9)
        {
            DrawEntirePlayingGameScreen = 2;
        }
        else if (ClearButtonPressedTimer == 5)
        {
            ClearButtonScale = 1;
            DrawEntirePlayingGameScreen = 2;
        }
    }
    else if (ClearButtonPressedTimer == 0)
    {
        ClearButtonPressedTimer = -1;

        ClearSelectedLetters();

        DrawEntirePlayingGameScreen = 2;
    }

    if (UndoButtonPressedTimer > -1)  UndoButtonPressedTimer--;

    if (UndoButtonScale == 0.75)
    {
        if (UndoButtonPressedTimer == 5)
        {
            UndoButtonScale = 1;
            DrawEntirePlayingGameScreen = 2;
        }
    }
    else if (UndoButtonPressedTimer == 0)
    {
        UndoButtonPressedTimer = -1;

        int indexToUndo = 0;
        for (int index = 0; index < 11; index++)
        {
            if (SelectedLetterPlayfieldX[index] != -1 && SelectedLetterPlayfieldY[index] != -1)
                indexToUndo = index;
            else  index = 999;
        }

        SelectedLetterPlayfieldX[indexToUndo] = -1;
        SelectedLetterPlayfieldY[indexToUndo] = -1;

        DrawEntirePlayingGameScreen = 2;
    }

    if (CheckButtonPressedTimer > -1)  CheckButtonPressedTimer--;

    if (CheckButtonScale == 0.75)
    {
        if (CheckButtonPressedTimer == 5)
        {
            CheckButtonScale = 1;

            if ( CheckSelectedLettersForWord() == false )
            {
                CheckButtonRedHue = 255;
                CheckButtonGreenHue = 0;
                CheckButtonBlueHue = 0;

                audio->PlayDigitalSoundFX(6, 0);
            }
            else if ( CheckSelectedLettersForWord() == true )
            {
                CheckButtonRedHue = 0;
                CheckButtonGreenHue = 255;
                CheckButtonBlueHue = 0;

                ClearWordTransparency = 254;

                audio->PlayDigitalSoundFX(5, 0);
            }

            DrawEntirePlayingGameScreen = 2;
        }
    }
    else if (CheckButtonPressedTimer == 0)
    {
        CheckButtonPressedTimer = -1;

        CheckButtonRedHue = 255;
        CheckButtonGreenHue = 255;
        CheckButtonBlueHue = 255;

        DrawEntirePlayingGameScreen = 2;
    }

    if (ClearWordTransparency < 255)
    {
        DrawEntirePlayingGameScreen = 2;

        if (ClearWordTransparency > 9)  ClearWordTransparency-=10;
        else
        {
            ClearSelectedLettersInPlayfield();
            ClearSelectedLetters();
            ClearWordTransparency = 255;
        }
    }
    else if ( ApplyGravityToPlayfield() == true )
    {

    }
    else if (ExplosionScale > -1)
    {
        DrawEntirePlayingGameScreen = 2;

        if (ExplosionScale < 20)
        {
            ExplosionScale+=0.5f;
        }
        else if (ExplosionScale == 20)
        {
            ExplosionScale+=0.5f;

            DeleteLettersInPlayfieldFromExplosion();
        }
        else if (ExplosionScale < 40)
        {
            ExplosionScale+=0.5;
            if (ExplosionTransparency > 9)  ExplosionTransparency-=10;
            else  ExplosionTransparency = 0;
        }
        else
        {
            ExplosionScale = -1;
            SetupNextLetters();
        }
    }
    else if (DroppingFast == true)
    {
        if (DownArrowPressedTimer > -1)  DownArrowPressedTimer--;
        else  DownArrowScale = 1;

        if (Playfield[ FallingLettersPlayfieldX[0] ][ FallingLettersPlayfieldY[0]+1 ] == 0)
        {
            FallingLettersScreenY[0]+=30;
            FallingLettersScreenY[1]+=30;
            FallingLettersScreenY[2]+=30;

            FallingLettersPlayfieldY[0]++;
            FallingLettersPlayfieldY[1]++;
            FallingLettersPlayfieldY[2]++;
        }
        if (Playfield[ FallingLettersPlayfieldX[0] ][ FallingLettersPlayfieldY[0]+1 ] > 0)
        {
            Playfield[ FallingLettersPlayfieldX[0] ][ FallingLettersPlayfieldY[0] ] = FallingLetters[0];
            Playfield[ FallingLettersPlayfieldX[1] ][ FallingLettersPlayfieldY[1] ] = FallingLetters[1];
            Playfield[ FallingLettersPlayfieldX[2] ][ FallingLettersPlayfieldY[2] ] = FallingLetters[2];

            DroppingFast = false;

            DownArrowScale = 1;

            if (FallingLetters[0] == 255)
            {
                ExplosionScale = 0;
                audio->PlayDigitalSoundFX(1, 0);
            }
            else  SetupNextLetters();
        }
    }
    else
    {
        if (input->MouseButtonPressed[0] == true)
        {
            int tileX = 27;
            int tileY = 26-(30*3);
            for (int y = 0; y < 17; y++)
            {
                for (int x = 0; x < 11; x++)
                {
                    if (  input->MouseY > ( tileY - (visuals->Sprites[300].TextureHeightOriginal/2) )
                       && input->MouseY < ( tileY + (visuals->Sprites[300].TextureHeightOriginal/2) )
                       && input->MouseX > ( tileX - (visuals->Sprites[300].TextureWidthOriginal/2) )
                       && input->MouseX < ( tileX + (visuals->Sprites[300].TextureWidthOriginal/2) )  )
                    {
                        bool letterNotAlreadySelected = false;
                        for (int index = 0; index < 11; index++)
                        {
                            if (SelectedLetterPlayfieldX[index] == x && SelectedLetterPlayfieldY[index] == y)
                                letterNotAlreadySelected = true;
                        }

                        if (letterNotAlreadySelected == false && Playfield[x][y] > 0)
                        {
                            for (int index = 0; index < 11; index++)
                            {
                                if (SelectedLetterPlayfieldX[index] == -1 && SelectedLetterPlayfieldY[index] == -1)
                                {
                                    DrawEntirePlayingGameScreen = 2;

                                    SelectedLetterPlayfieldX[index] = x;
                                    SelectedLetterPlayfieldY[index] = y;

                                    index = 999;

                                    audio->PlayDigitalSoundFX(0, 0);
                                }
                            }
                        }
                    }

                    tileX+=41;
                }

                tileX = 27;
                tileY+=30;
            }

            if (  input->MouseY > ( visuals->Sprites[105].ScreenY - (visuals->Sprites[105].TextureHeightOriginal/2) )
               && input->MouseY < ( visuals->Sprites[105].ScreenY + (visuals->Sprites[105].TextureHeightOriginal/2) )
               && input->MouseX > ( visuals->Sprites[105].ScreenX - (visuals->Sprites[105].TextureWidthOriginal/2) )
               && input->MouseX < ( visuals->Sprites[105].ScreenX + (visuals->Sprites[105].TextureWidthOriginal/2) )  )
            {
                if (DownArrowScale == 1)
                {
                    if (FallingLettersPlayfieldY[0] > 3
                       && DroppingFast == false)
                    {
                        audio->PlayDigitalSoundFX(0, 0);

                        DownArrowScale = 0.75;
                        DrawEntirePlayingGameScreen = 2;
                        DownArrowPressedTimer = 10;

                        DroppingFast = true;

                        if (FallingLettersOffsetY > 0)
                        {
                            int offsetYMax;
                            if (FallingLetters[0] == 255)  offsetYMax = 33;
                            else  offsetYMax = 35;
                            for (int offsetY = FallingLettersOffsetY; offsetY < offsetYMax; offsetY+=static_cast<int>(movementY))
                            {
								FallingLettersScreenY[0] += static_cast<int>(movementY);
								FallingLettersScreenY[1] += static_cast<int>(movementY);
								FallingLettersScreenY[2] += static_cast<int>(movementY);
                            }
                        }

                        if (Playfield[ FallingLettersPlayfieldX[0] ][ FallingLettersPlayfieldY[0]+1 ] == 0)
                        {
                            FallingLettersPlayfieldY[0]++;
                            FallingLettersPlayfieldY[1]++;
                            FallingLettersPlayfieldY[2]++;
                        }
                        else if (Playfield[ FallingLettersPlayfieldX[0] ][ FallingLettersPlayfieldY[0]+1 ] > 0)
                        {
                            Playfield[ FallingLettersPlayfieldX[0] ][ FallingLettersPlayfieldY[0] ] = FallingLetters[0];
                            Playfield[ FallingLettersPlayfieldX[1] ][ FallingLettersPlayfieldY[1] ] = FallingLetters[1];
                            Playfield[ FallingLettersPlayfieldX[2] ][ FallingLettersPlayfieldY[2] ] = FallingLetters[2];

                            DroppingFast = false;

                            SetupNextLetters();

                            DownArrowScale = 1;
                        }
                    }
                }
            }
            else if (  input->MouseY > ( visuals->Sprites[100].ScreenY - (visuals->Sprites[100].TextureHeightOriginal/2) )
               && input->MouseY < ( visuals->Sprites[100].ScreenY + (visuals->Sprites[100].TextureHeightOriginal/2) )
               && input->MouseX > ( visuals->Sprites[100].ScreenX - (visuals->Sprites[100].TextureWidthOriginal/2) )
               && input->MouseX < ( visuals->Sprites[100].ScreenX + (visuals->Sprites[100].TextureWidthOriginal/2) )  )
            {
                if (LeftArrowScale == 1)
                {
                    if (FallingLetters[0] == 255)
                    {
                        audio->PlayDigitalSoundFX(0, 0);

                        LeftArrowScale = 0.75;
                        DrawEntirePlayingGameScreen = 2;
                        LeftArrowPressedTimer = 10;

                        if (FallingLettersPlayfieldX[0] > -1)
                        {
                            FallingLettersPlayfieldX[0]--;

                            FallingLettersScreenX[0]-=41;
                        }
                    }
                }
            }
            else if (  input->MouseY > ( visuals->Sprites[110].ScreenY - (visuals->Sprites[110].TextureHeightOriginal/2) )
               && input->MouseY < ( visuals->Sprites[110].ScreenY + (visuals->Sprites[110].TextureHeightOriginal/2) )
               && input->MouseX > ( visuals->Sprites[110].ScreenX - (visuals->Sprites[110].TextureWidthOriginal/2) )
               && input->MouseX < ( visuals->Sprites[110].ScreenX + (visuals->Sprites[110].TextureWidthOriginal/2) )  )
            {
                if (RightArrowScale == 1)
                {
                    if (FallingLetters[0] == 255)
                    {
                        audio->PlayDigitalSoundFX(0, 0);

                        RightArrowScale = 0.75;
                        DrawEntirePlayingGameScreen = 2;
                        RightArrowPressedTimer = 10;

                        if (FallingLettersPlayfieldX[0] < 10)
                        {
                            FallingLettersPlayfieldX[0]++;

                            FallingLettersScreenX[0]+=41;
                        }
                    }
                }
            }
            else if (  input->MouseY > ( visuals->Sprites[95].ScreenY - (visuals->Sprites[95].TextureHeightOriginal/2) )
               && input->MouseY < ( visuals->Sprites[95].ScreenY + (visuals->Sprites[95].TextureHeightOriginal/2) )
               && input->MouseX > ( visuals->Sprites[95].ScreenX - (visuals->Sprites[95].TextureWidthOriginal/2) )
               && input->MouseX < ( visuals->Sprites[95].ScreenX + (visuals->Sprites[95].TextureWidthOriginal/2) )  )
            {
                if (BombButtonScale == 1 && NextLetters[0] != 255)
                {
                    if (input->MouseButtonPressed[0] == true && Bombs > 0)
                    {
                        audio->PlayDigitalSoundFX(4, 0);

                        DrawEntirePlayingGameScreen = 2;

                        BombButtonPressedTimer = 20;
                    }
                }
            }
            else if (  input->MouseY > ( visuals->Sprites[115].ScreenY - (visuals->Sprites[115].TextureHeightOriginal/2) )
               && input->MouseY < ( visuals->Sprites[115].ScreenY + (visuals->Sprites[115].TextureHeightOriginal/2) )
               && input->MouseX > ( visuals->Sprites[115].ScreenX - (visuals->Sprites[115].TextureWidthOriginal/2) )
               && input->MouseX < ( visuals->Sprites[115].ScreenX + (visuals->Sprites[115].TextureWidthOriginal/2) )  )
            {
                if (ClearButtonScale == 1)
                {
                    if (SelectedLetterPlayfieldX[1] != -1 && SelectedLetterPlayfieldY[1] != -1)
                    {
                        audio->PlayDigitalSoundFX(3, 0);

                        ClearButtonScale = 0.75;
                        ClearButtonPressedTimer = 10;
                    }
                }
            }
            else if (  input->MouseY > ( visuals->Sprites[125].ScreenY - (visuals->Sprites[125].TextureHeightOriginal/2) )
               && input->MouseY < ( visuals->Sprites[125].ScreenY + (visuals->Sprites[125].TextureHeightOriginal/2) )
               && input->MouseX > ( visuals->Sprites[125].ScreenX - (visuals->Sprites[125].TextureWidthOriginal/2) )
               && input->MouseX < ( visuals->Sprites[125].ScreenX + (visuals->Sprites[125].TextureWidthOriginal/2) )  )
            {
                if (UndoButtonScale == 1)
                {
                    if (SelectedLetterPlayfieldX[0] != -1 && SelectedLetterPlayfieldY[0] != -1)
                    {
                        audio->PlayDigitalSoundFX(2, 0);

                        UndoButtonScale = 0.75;
                        DrawEntirePlayingGameScreen = 2;
                        UndoButtonPressedTimer = 10;
                    }
                }
            }
            else if (  input->MouseY > ( visuals->Sprites[120].ScreenY - (visuals->Sprites[120].TextureHeightOriginal/2) )
               && input->MouseY < ( visuals->Sprites[120].ScreenY + (visuals->Sprites[120].TextureHeightOriginal/2) )
               && input->MouseX > ( visuals->Sprites[120].ScreenX - (visuals->Sprites[120].TextureWidthOriginal/2) )
               && input->MouseX < ( visuals->Sprites[120].ScreenX + (visuals->Sprites[120].TextureWidthOriginal/2) )  )
            {
                if (CheckButtonScale == 1)
                {
                    if (SelectedLetterPlayfieldX[2] != -1 && SelectedLetterPlayfieldY[2] != -1)
                    {
                        CheckButtonScale = 0.75;
                        DrawEntirePlayingGameScreen = 2;
                        CheckButtonPressedTimer = 10;
                    }
                }
            }
            else if (  input->MouseY > ( visuals->Sprites[55].ScreenY - (visuals->Sprites[55].TextureHeightOriginal/2) )
               && input->MouseY < ( visuals->Sprites[55].ScreenY + (visuals->Sprites[55].TextureHeightOriginal/2) )
               && input->MouseX > ( visuals->Sprites[55].ScreenX - (visuals->Sprites[55].TextureWidthOriginal/2) )
               && input->MouseX < ( visuals->Sprites[55].ScreenX + (visuals->Sprites[55].TextureWidthOriginal/2) )  )
            {
                audio->PlayDigitalSoundFX(1, 0);
                screens->ScreenTransitionStatus = FadeOut;
            }
        }

        if (FallingLettersOffsetY == 0)
        {
            if (Playfield[ FallingLettersPlayfieldX[0] ][ FallingLettersPlayfieldY[0]+1 ] == 0)
            {
				FallingLettersOffsetY = static_cast<int>(movementY);

				FallingLettersScreenY[0] += static_cast<int>(movementY);
				FallingLettersScreenY[1] += static_cast<int>(movementY);
				FallingLettersScreenY[2] += static_cast<int>(movementY);

                FallingLettersPlayfieldY[0]++;
                FallingLettersPlayfieldY[1]++;
                FallingLettersPlayfieldY[2]++;
            }
            else if (Playfield[ FallingLettersPlayfieldX[0] ][ FallingLettersPlayfieldY[0]+1 ] > 0)
            {
                if (FallingLetters[0] == 255)
                {
                    ExplosionScale = 0.0f;
                    audio->PlayDigitalSoundFX(1, 0);
                }
                else
                {
                    Playfield[ FallingLettersPlayfieldX[0] ][ FallingLettersPlayfieldY[0] ] = FallingLetters[0];
                    Playfield[ FallingLettersPlayfieldX[1] ][ FallingLettersPlayfieldY[1] ] = FallingLetters[1];
                    Playfield[ FallingLettersPlayfieldX[2] ][ FallingLettersPlayfieldY[2] ] = FallingLetters[2];

                    audio->PlayDigitalSoundFX(10, 0);

                    SetupNextLetters();
                }
            }
        }
        else if (FallingLettersOffsetY < 30)
        {
			FallingLettersScreenY[0] += static_cast<int>(movementY);
			FallingLettersScreenY[1] += static_cast<int>(movementY);
			FallingLettersScreenY[2] += static_cast<int>(movementY);

			FallingLettersOffsetY += static_cast<int>(movementY);
        }
        else if (FallingLettersOffsetY == 30)
        {
            FallingLettersOffsetY = 0;
        }
    }
}
