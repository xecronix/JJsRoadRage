#include "high_score_sim.h"
#include "gfx_manager.h"
#include "sfx_manager.h"
#include "xx_types.h"
#include "xx_input.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_SCORES 5
#define SCORES_FILE "assets/scores"


GameState *game;
static int scores[MAX_SCORES];
static int scoresLoaded = 0;
static int scoreChecked = 0;

int LoadHighScores(void)
{
	if (scoresLoaded != 0) {return 1;} // scores already loaded.
	for (int i = 0; i < MAX_SCORES; i++)
	{
		// make sure we have default values.  Even if we can't open a file.
		scores[i] = 0;	
	}

	// open a file for the scores or create it if doesn't exist.
	char buf[16];
	buf[0]= '\0';
	const int bufSize = sizeof(buf) / sizeof(buf[0]);
	FILE *fp = fopen(SCORES_FILE, "r+");
	if (!fp) { fp = fopen(SCORES_FILE, "w+"); }
	if (!fp) {printf("could not open or create file %s", SCORES_FILE); return 0;}
	
	// we have a file, let's try to read it.  
	for (int i = 0; i < MAX_SCORES; i++)
	{
		if (!fgets(buf, bufSize, fp )){scores[i] = 0;} // we can't read the file at this point. set default.
		else 
		{
			int num	=atoi(buf);
			scores[i] = num;
		}
	}
	fclose(fp);
	scoresLoaded = 1;
	return 1;
}

int SaveHighScore(void)
{
	//scoreChecked is a file level static variable
	if (game->run_stats.score == scoreChecked){return 1;}
	scoreChecked = game->run_stats.score;
	int saveScore = 0;
	int nextScore = game->run_stats.score;
	for (int i = 0; i < MAX_SCORES; i++)	
	{
		if (i > 0)
		{
			int lastScore = scores[i-1];
			if (nextScore == lastScore){break;}	
		}

		int tempScore = scores[i];
		if (nextScore > scores[i])
		{
			scores[i] = nextScore;
			nextScore = tempScore;
			saveScore = 1;
		}
	}
	
	if (saveScore == 0) {return 1;}
	FILE *fp = fopen(SCORES_FILE, "r+");
	if (!fp) {printf("could not open scores file %s", SCORES_FILE); return 0;}
	
	for (int i = 0; i < MAX_SCORES; i++)	
	{
		char buf[20];
		sprintf(buf, "%d", scores[i]);
		fputs(buf  ,fp);
    	fputs("\n" ,fp);		
	}
	fclose(fp);
	return 1;
}

int HighScoreSim(GameState *gs)
{
	game = gs;
    if (gs->screen_id != SCREEN_SCORES) return 0;

    if (IsKBPressed(KC_BACKSPACE))
	{
		gs->screen_id = SCREEN_START;
		Sfx_PlayMenuAccept();
	}

	LoadHighScores();
	SaveHighScore();

    int x = WIN_W / 4;
    int y = WIN_H / 4;
    int w = WIN_W / 2;
    int h = WIN_H / 2;

    XXDrawRectangle(x, y, w, h, XXRGBA(128, 0, 128, 128));
    XXDrawRectangleOutline(x, y, w, h, 2, XX_BLACK);

	const char *lines[6]; int numLine = 6; 
	int padding = 5; 
	int fontSize = 20; 
	int lineSize = fontSize + padding;

	lines[0] = "High Scores:";
	lines[1] = "  %d";
	lines[2] = "  %d";
	lines[3] = "  %d";
	lines[4] = "  %d";
	lines[5] = "  %d";

	int tw = XXMeasureText(lines[0], fontSize);
    
    int textX = x + (w - tw) / 2;
    int textY = y + (h / 2) - ( lineSize * numLine / 2);
	for (int i = 0; i < numLine; i++)
	{
		if (i == 0) {XXDrawText(lines[i], textX, textY, fontSize, XX_WHITE);}
		else
		{
			char buf[32];
			sprintf(buf, lines[i], scores[i-1]);
			XXDrawText(buf, textX, textY, fontSize, XX_WHITE);
		}
		textY+=lineSize;
	}
	
	XXDrawText("<<-- [BACKSPACE]", x + 25, y + 25, fontSize, XX_WHITE);
	return 1; 
}
