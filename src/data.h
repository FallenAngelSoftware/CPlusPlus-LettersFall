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
#ifndef DATA
#define DATA

#include <fstream>
#include <sstream>

class Data
{
public:

	Data(void);
	virtual ~Data(void);

    int NumberOfPlays;

    char HighScoresName[5][10][20];
    Uint8 HighScoresLevel[5][10];
    Uint32 HighScoresScore[5][10];

    bool NewHighScoreRegistered;

    int NameInputArayIndex;

    Uint8 PlayerRankOnGameOver;

    int NameInputJoyCharX;
    int NameInputJoyCharY;
    char NameInputJoyChar;

    void CheckForNewHighScore(void);

    void ClearHighScores(void);

    char CharRead[5000];
    int CharReadIndexStart;
    int CharReadIndexEnd;
    void GetDataFromFile(const char* fileName, std::stringstream& ss);
    void LoadHighScoresAndOptions(void);

    void SaveHighScoresAndOptions(void);

    void DisplayWebsite(void);
};

#endif
