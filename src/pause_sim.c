#include "pause_sim.h"
#include "gfx_manager.h"
#include "xx_types.h"
#include "xx_input.h"

int PauseSim(GameState *gs)
{
    if (gs->run_mode != RUNMODE_PAUSED) return 0;
    if (IsKBPressed(KC_R)){gs->run_mode = RUNMODE_RUNNING; return 1;}
    if (IsKBPressed(KC_Q)) {gs->run_mode = RUNMODE_QUIT; return 1;}

    int x = WIN_W / 4;
    int y = WIN_H / 4;
    int w = WIN_W / 2;
    int h = WIN_H / 2;

    XXDrawRectangle(x, y, w, h, XXRGBA(128, 0, 128, 128));
    XXDrawRectangleOutline(x, y, w, h, 2, XX_BLACK);

    const char *buf =  "Press 'R' to [R]esume";
    const char *buf2 = "Press 'Q' to [Q]uit";
    int fontSize = 20;
    int tw = XXMeasureText(buf, fontSize);
    
    int textX = x + (w - tw) / 2;
    int textY = y + (h / 2) - (fontSize / 2);
    XXDrawText(buf, textX, textY, fontSize, XX_WHITE);
    XXDrawText(buf2, textX, textY + fontSize + 5, fontSize, XX_WHITE);
    return 1;
}
