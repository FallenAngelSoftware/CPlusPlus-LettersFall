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
#ifndef LOGIC
#define LOGIC

class Logic
{
public:

	Logic(void);
	virtual ~Logic(void);

    bool RunLoop;

    #define ChildMode                   0
    #define TeenagerMode                1
    #define AdultMode                   2
    #define TimeAttack15MinuteMode      3
    #define StoryMode                   4
    int GameMode;

    int SelectedBackground;
    int SelectedMusicTrack;

    bool MusicJukebox;

    bool Shareware;

    bool PAUSEgame;

    int GameOver;

    #define NumberOfLetters     26

    Uint32 Score;
    Uint16 Level;
    Uint16 Bombs;

    Uint8 Playfield[11][18];

    Uint8 NextLetters[3];
    Uint8 FallingLetters[3];

    int FallingLettersPlayfieldX[3];
    int FallingLettersPlayfieldY[3];
    int FallingLettersScreenX[3];
    int FallingLettersScreenY[3];
    int FallingLettersOffsetY;
    Uint8 FallingLettersNextPlayfieldX;

    float DownArrowScale;
    int DownArrowPressedTimer;
    float LeftArrowScale;
    int LeftArrowPressedTimer;
    float RightArrowScale;
    int RightArrowPressedTimer;

    bool DroppingFast;

    int BombButtonPressedTimer;
    float BombButtonScale;
    float ExplosionScale;
    Uint8 ExplosionTransparency;

    int SelectedLetterPlayfieldX[11];
    int SelectedLetterPlayfieldY[11];

    float ClearButtonScale;
    int ClearButtonPressedTimer;
    float UndoButtonScale;
    int UndoButtonPressedTimer;

    float CheckButtonScale;
    int CheckButtonPressedTimer;
    Uint8 CheckButtonRedHue;
    Uint8 CheckButtonGreenHue;
    Uint8 CheckButtonBlueHue;

    Uint8 ClearWordTransparency;

    int BombBonusMeterValue;
    int LevelAdvancementValue;

    int DrawEntirePlayingGameScreen;

    Uint32 InitialGameSpeed[5];
    Uint32 CurrentGameSpeed;

    Uint32 TimeAttackMinutes;
    Uint32 TimeAttackSeconds;
    Uint32 TimeAttackNextSecondTick;

    Uint8 GetRandomLetter(void);

    void SetupNextLetters(void);

    void SetupForNewGame(void);

    void DeleteLettersInPlayfieldFromExplosion(void);

    bool ApplyGravityToPlayfield(void);

    void GetDataFromFile(const char* fileName, std::stringstream& ss);
    bool CheckSelectedLettersForWord(void);

    void ClearSelectedLetters(void);
    void ClearSelectedLettersInPlayfield(void);

    void RunGameplayEngine(void);
};

#endif
