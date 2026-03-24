#include "settings_sim.h"
#include "gfx_manager.h"
#include "sfx_manager.h"
#include "xx_types.h"
#include "xx_input.h"
#include <stdio.h>

int SettingsSim(GameState *gs)
{
    if (gs->screen_id != SCREEN_SETTINGS) return 0;
    if (IsKBPressed(KC_BACKSPACE))
	{
        gs->screen_id = SCREEN_START;
		Sfx_PlayMenuAccept();
	}

	if (IsKBPressed(KC_ONE)   || IsKBPressed(KC_KP_1)){gs->difficulty = DIFF_WIMP; Sfx_PlayMenuChange();}
	if (IsKBPressed(KC_TWO)   || IsKBPressed(KC_KP_2)){gs->difficulty = DIFF_EASY; Sfx_PlayMenuChange();}
	if (IsKBPressed(KC_THREE) || IsKBPressed(KC_KP_3)){gs->difficulty = DIFF_HARD; Sfx_PlayMenuChange();}
	if (IsKBPressed(KC_FOUR)  || IsKBPressed(KC_KP_4)){gs->difficulty = DIFF_BOSS; Sfx_PlayMenuChange();}
    if (IsKBPressed(KC_F))     {gs->screen_view = SCRVIEW_FULLSCREEN; Sfx_PlayMenuChange();}
	if (IsKBPressed(KC_W))     {gs->screen_view = SCRVIEW_WINDOW;     Sfx_PlayMenuChange();}

    int x = WIN_W / 4;
    int y = WIN_H / 4;
    int w = WIN_W / 2;
    int h = WIN_H / 2;

    XXDrawRectangle(x, y, w, h, XXRGBA(128, 0, 128, 128));
    XXDrawRectangleOutline(x, y, w, h, 2, XX_BLACK);

	const char *lines[9]; int numLine = 9; 
	int padding = 5; 
	int fontSize = 20; 
	int lineSize = fontSize + padding;

    lines[0] =  "Choose Your Expertise : [%d]";
	lines[1] =  "  [1] Wimp";
	lines[2] =  "  [2] Easy";
	lines[3] =  "  [3] Hard";
	lines[4] =  "  [4] Boss";
    lines[5] =  "  ";
    lines[6] =  "Choose a view : [%c]";
    lines[7] =  "  [F] Fullscreen";
    lines[8] =  "  [W] Window";
    
    int tw = XXMeasureText(lines[0], fontSize);
    int textX = x + (w - tw) / 2;
    int textY = y + (h / 2) - ( lineSize * numLine / 2);
	for (int i = 0; i < numLine; i++)
	{
		if (i == 0)
		{
			char buf[64];
			sprintf(buf, lines[0], gs->difficulty);
			XXDrawText(buf, textX, textY, fontSize, XX_WHITE); 
		}
        else if (i == 6)
        {
			char buf[64];
			sprintf(buf, lines[6], gs->screen_view);
			XXDrawText(buf, textX, textY, fontSize, XX_WHITE); 
        }
		else {XXDrawText(lines[i], textX, textY, fontSize, XX_WHITE);}
		textY+=lineSize;
	}

	XXDrawText("<<-- [BACKSPACE]", x + 25, y + 25, fontSize, XX_WHITE);
	
	return 1; 
}
