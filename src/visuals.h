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
#ifndef VISUALS
#define VISUALS

#include <fstream>
#include <sstream>

class Visuals
{
public:

	Visuals(void);
	virtual ~Visuals(void);

    bool CoreFailure;

    bool ForceAspectRatio;

    bool FullScreenMode;

    SDL_Window *Window;
    int WindowWidthCurrent;
    int WindowHeightCurrent;
    SDL_Renderer *Renderer;

    Uint32 FrameLock;
    Uint32 SystemTicks;
    Uint32 NextFrameTicks;
    Uint32 NumberOfFrames;
    Uint8 CurrentFramePerSecond;
    Uint32 FramesPerSecondArray[10];
    Uint32 NextSecondTick;
    Uint32 AverageFPS;

    void CalculateFramerate(void);
    void ProcessFramerate(void);

    bool InitializeWindow(void);

    void ClearScreenBufferWithColor(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);

    int TotalNumberOfLoadedStaffTexts;

    char TextBufferImage[256];

    #define NumberOfSprites     1300
    struct Sprite
    {
        SDL_Texture *Texture;
        int ScreenX;
        int ScreenY;
        float ScaleX;
        float ScaleY;
        double RotationDegree;
        Uint8 RedHue;
        Uint8 GreenHue;
        Uint8 BlueHue;
        Uint8 Transparency;
        bool Smooth;
        bool FlipX;
        bool FlipY;
        int TextureWidthOriginal;
        int TextureHeightOriginal;
        int AnimationTimer;
    } Sprites[NumberOfSprites];

    void SetupCurrentLineImageReadFromFile(void);
    bool LoadSpritesAndInitialize(void);

    bool LoadInterfaceIntoMemory(void);
    bool LoadCharacterTilesIntoMemory(void);

    bool LoadStaffTextIntoMemory(const char *staffText, Uint8 textBlue);
    bool PreloadStaffTextsIntoMemory(void);

    void DrawSpriteOntoScreenBuffer(Uint16 index);

    #define JustifyLeft             0
    #define JustifyCenter           1
    #define JustifyRight            2
    #define JustifyCenterOnPoint    3

    #define NumberOfFonts           5
    TTF_Font *Font[NumberOfFonts];
    char VariableText[64];

    int TextCacheCurrentIndex;
    #define NumberOfTextsCached     100
    SDL_Texture *TextTexture[NumberOfTextsCached];
    SDL_Texture *TextOutlineTexture[NumberOfTextsCached];
    char TextCachedText[NumberOfTextsCached][64];
    int TextCachedScreenX[NumberOfTextsCached];
    int TextCachedScreenY[NumberOfTextsCached];
    int TextCachedWidth[NumberOfTextsCached];
    int TextCachedHeight[NumberOfTextsCached];

    void GetDataFromFile(const char* fileName, std::stringstream& ss);

    bool LoadFontsIntoMemory(void);
    void UnloadFontsFromMemory(void);

    void ClearTextCache(void);
    void DrawTextOntoScreenBuffer(const char *textToDisplay, TTF_Font *font, int posX, int posY
                                 , Uint8 XJustification, Uint8 textRed, Uint8 textGreen, Uint8 textBlue
                                 , Uint8 outlineRed, Uint8 outlineGreen, Uint8 outlineBlue);
};

#endif
