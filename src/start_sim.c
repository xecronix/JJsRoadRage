#include "start_sim.h"
#include "gfx_manager.h"
#include "sfx_manager.h"
#include "xx_types.h"
#include "xx_input.h"
#include <stdio.h>
int StartSim(GameState *gs)
{
    if (gs->screen_id != SCREEN_START) return 0;

    if (IsKBPressed(KC_TAB))
	{
        gs->run_mode = RUNMODE_RUNNING;
		gs->screen_id = SCREEN_PLAY;
	}

	if (IsKBPressed(KC_S))
	{
		gs->screen_id = SCREEN_SETTINGS;
		Sfx_PlayMenuAccept();
	}

    int x = WIN_W / 4;
    int y = WIN_H / 4;
    int w = WIN_W / 2;
    int h = WIN_H / 2;

    XXDrawRectangle(x, y, w, h, XXRGBA(128, 0, 128, 128));
    XXDrawRectangleOutline(x, y, w, h, 2, XX_BLACK);

	const char *lines[4]; int numLine = 4; 
	int padding = 5; 
	int fontSize = 20; 
	int lineSize = fontSize + padding;

	lines[0] =  "Let's Do This!";
    lines[1] =  "  Press [TAB] To Start";
	lines[2] =  "  Press [S] For Settings";
	lines[3] =  "  Press [ESC] To Go Do Something Lame";

    int tw = XXMeasureText(lines[3], fontSize);
    
    int textX = x + (w - tw) / 2;
    int textY = y + (h / 2) - ( lineSize * numLine / 2);
	for (int i = 0; i < numLine; i++)
	{
		XXDrawText(lines[i], textX, textY, fontSize, XX_WHITE);
		textY+=lineSize;
	}
	
	textY = y + lineSize;
	XXDrawText("[TAB] Key To Shoot. Arrow Keys To Move.", x + 25, textY, fontSize, XX_WHITE);
	textY += lineSize;
	XXDrawText("Score: Red Cars: 20, Green Cars: -10", x + 25, textY, fontSize, XX_WHITE);
	return 1; 
}
