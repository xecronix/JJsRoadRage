#include "game_sim.h"
#include "actor.h"
#include "actor_templates.h"
#include "actor_manager.h"
#include "gfx_manager.h"
#include "sfx_manager.h"
#include "xx_types.h"
#include "xx_input.h"
#include "xx_conf.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

enum { TILE_G = 0, TILE_R = 1, TILE_S = 2, TILE_STRIPE = 3 };

static TileRec TileMap[MAP_ROW * 3][MAP_COL * 3];
static ImageAsset gTileSheet;
static ImageAsset gTileAssets[TILE_SHEET_SIZE];
static const char* _level1TileSheetFile  = "assets/level1/tiles.png";
static int x_offset = 0;
static int y_offset = 0;
static int mapSpeed = 7; 
static FrameView frameView = {0};
static int shotCooler = 0;
static Actor *Hero;
static Actor *Shots[MAX_SHOTS];
static Actor *GreenPowerUp;
static Actor *RedPowerUp;
static ExplosionParticles Explosions[MAX_EXPLOSIONS];
static const int TRAFFIC_KINDS[] = { ACT_KIND_ENEMY, ACT_KIND_CIVILIAN };
static int greenPowerUpScore = INITIAL_GREEN_PWRUP;
static int redPowerUpScore = INITIAL_RED_PWRUP;
static long long greenPowerUpExpire_ms;

static int doInit = 1;
static GameState *gs;

static XXColor TimerColors[6];

int HeroGameOver(void);
int BlowUpActor(Actor *a);


int LoadPowerUps(void)
{
	GreenPowerUp = ActMan_Create(ACT_KIND_POWER_UP);
	if (!LoadAssetFromFile(&GreenPowerUp->imgAsset, "assets/level1/green_pwrup.png")) 
	{
        return 0;
    }
	GreenPowerUp->imgAsset.imgSize.w = 64;
	GreenPowerUp->imgAsset.imgSize.h = 64;
    
    
    RedPowerUp = ActMan_Create(ACT_KIND_POWER_UP);
	if (!LoadAssetFromFile(&RedPowerUp->imgAsset, "assets/level1/red_pwrup.png")) 
	{
        return 0;
    }
	RedPowerUp->imgAsset.imgSize.w = 64;
	RedPowerUp->imgAsset.imgSize.h = 64;
    return 1;
}

int LoadPlayer(void)
{
	Hero = ActMan_Create(ACT_KIND_HERO);
	if (!LoadAssetFromFile(&Hero->imgAsset, "assets/level1/hero.png")) 
	{
        return 0;
    }
	Hero->imgAsset.imgSize.w = 64;
	Hero->imgAsset.imgSize.h = 64;
    return 1;
}

int LoadShots(void)
{
	ImageAsset img = {0};
	if (!LoadAssetFromFile(&img, "assets/level1/shots.png")) 
	{
        return 0;
    }

    img.imgSize.w = 64;
    img.imgSize.h = 64;

    for (int i = 0; i < MAX_SHOTS; i++) 
	{
        Actor *a = ActMan_Create(ACT_KIND_HEROSHOT);
        if (!a) 
		{
            /* pool full: stop early, still counts as partial success */
            break;
        }

        a->imgAsset = img;   /* struct copy (easy and correct) */
		a->speed = 10;
		a->position.y = WIN_H;
		Shots[i] = a;
    }

    return 1;
}

int LoadEnemy(void)
{
    ImageAsset img = {0};

    if (!LoadAssetFromFile(&img, "assets/level1/baddie1.png")) 
	{
        return 0;
    }

    img.imgSize.w = 64;
    img.imgSize.h = 64;

    srand(time(NULL));
    for (int i = 0; i < MAX_ENEMY; i++) 
	{
        Actor *a = ActMan_Create(ACT_KIND_ENEMY);
        if (!a) 
		{
            /* pool full: stop early, still counts as partial success */
            break;
        }

        a->imgAsset = img;   /* struct copy (easy and correct) */
		a->speed = (rand() % 3) + 3;
		a->position.y = WIN_H;
    }

    return 1;
}

int LoadCivilian(void)
{
    ImageAsset img = {0};

    if (!LoadAssetFromFile(&img, "assets/level1/civ1.png")) 
	{
        return 0;
    }

    img.imgSize.w = 64;
    img.imgSize.h = 64;

    /* let's make 24 of these */
	srand(time(NULL));
    for (int i = 0; i < MAX_CIVILIAN; i++) 
	{
        Actor *a = ActMan_Create(ACT_KIND_CIVILIAN);
        if (!a) 
		{
            /* pool full: stop early, still counts as partial success */
            break;
        }

        a->imgAsset = img;   /* struct copy (easy and correct) */
		a->speed = (rand() % 3) + 3;
		a->position.y = WIN_H;
    }

    return 1;
}


/*
	opens a file and reads the contents.  
	setup the TileMap with x, y, mapkey data
	return 0 on error or number of lines in the 
	file on success.
*/
int LoadMap(const char * strMapFilePath)
{
    int r, c;
	char line[1024];
	FILE *f = fopen(strMapFilePath, "rb");
    if (!f) { perror("fopen"); return 0; }
	for (r = 0; r < MAP_ROW; r++)
	{
        int y;
		if (fgets(line, sizeof(line), f) == NULL) 
		{
			printf("Left loop early row[%d]", r);
			break;
		}
		if (strlen(line) < MAP_COL) 
		{
			printf ("Map line too short: [%s] \n", line);
			return 0;
		}
		y = r * TILE_SIZE;	
		// if the map line is too long we're going to silently truncate... for now.
		for (c = 0; c < MAP_COL; c++)
		{			
			TileMap[r][c].x = c * TILE_SIZE;
			TileMap[r][c].y = y;
			TileMap[r][c].mapkey = line[c];
		}
 	}
	fclose(f);
	return r;
}
// this is used for debugging purposes.  It's a pretty view of the map.
void PrintMap(void)
{	
	int r, c;
	for (r = 0; r < MAP_ROW*3; r++)	
	{
		for (c = 0; c < MAP_COL*3; c++)
		{
			printf ("[%c: (%5d,%5d)]", TileMap[r][c].mapkey, TileMap[r][c].x, TileMap[r][c].y);
		}
		printf ("\n");
	}
}

static void BuildTileOffsets(int startGridOffsetX, int startGridOffsetY, int xPxOffset, int yPxOffset)
{
    for (int r = 0; r < MAP_ROW; r++) 
	{
        for (int c = 0; c < MAP_COL; c++) 
		{
            TileMap[startGridOffsetY + r][startGridOffsetX + c].x = (c * TILE_SIZE) + xPxOffset;
            TileMap[startGridOffsetY + r][startGridOffsetX + c].y = (r * TILE_SIZE) + yPxOffset;

            /* Copy base chunk data */
            TileMap[startGridOffsetY + r][startGridOffsetX + c].mapkey   = TileMap[r][c].mapkey;
            TileMap[startGridOffsetY + r][startGridOffsetX + c].imgAsset = TileMap[r][c].imgAsset;
        }
    }
}

// this will be a shameless green function for now.  
// it loads an atlas image of tiles
// creates an array of TILE_SHEET_SIZE images
// creates 2d texture
// adds the ID to the TileMap[][]
// Way too much work for one function.  
// returns 0 on failure.  or the number of cells filled with images.
int LoadMapTiles(void)
{
    int r, c, imgCnt = 0;

    /* Load the tile sheet ONCE */
    if (!LoadAssetFromFile(&gTileSheet, _level1TileSheetFile)) 
	{
        printf("ERROR: failed to load %s\n", _level1TileSheetFile);
        return 0;
    }

    /* Create 4 tile "views" into the same texture id */
    for (int i = 0; i < TILE_SHEET_SIZE; i++) 
	{
        gTileAssets[i].id = gTileSheet.id;

        gTileAssets[i].src.x = i * TILE_SIZE;
        gTileAssets[i].src.y = 0;

        gTileAssets[i].dest.x = 0;
        gTileAssets[i].dest.y = 0;

        gTileAssets[i].imgSize.w = TILE_SIZE;
        gTileAssets[i].imgSize.h = TILE_SIZE;

        gTileAssets[i].rotationPoint.x = 0;
        gTileAssets[i].rotationPoint.y = 0;
        gTileAssets[i].rotationAngle = 0.0f;
    }

    /* Assign assets into the base chunk */
    for (r = 0; r < MAP_ROW; r++) 
	{
        for (c = 0; c < MAP_COL; c++) 
	{
            char k = TileMap[r][c].mapkey;

            if      (k == 'G') TileMap[r][c].imgAsset = gTileAssets[TILE_G];
            else if (k == 'R') TileMap[r][c].imgAsset = gTileAssets[TILE_R];
            else if (k == 'S') TileMap[r][c].imgAsset = gTileAssets[TILE_S];
            else if (k == '|') TileMap[r][c].imgAsset = gTileAssets[TILE_STRIPE];
            else { printf("Found bad char in level map. Early Exit!\n"); return 0; }

            imgCnt++;
        }
    }

    /* Build the 3x3 neighborhood */
    BuildTileOffsets(MAP_COL * 0, MAP_ROW * 0, TILE_SIZE * MAP_COL * -1, TILE_SIZE * MAP_ROW * -1);
    BuildTileOffsets(MAP_COL * 1, MAP_ROW * 0, TILE_SIZE * MAP_COL *  0, TILE_SIZE * MAP_ROW * -1);
    BuildTileOffsets(MAP_COL * 2, MAP_ROW * 0, TILE_SIZE * MAP_COL *  1, TILE_SIZE * MAP_ROW * -1);

    BuildTileOffsets(MAP_COL * 0, MAP_ROW * 1, TILE_SIZE * MAP_COL * -1, TILE_SIZE * MAP_ROW *  0);
    BuildTileOffsets(MAP_COL * 1, MAP_ROW * 1, TILE_SIZE * MAP_COL *  0, TILE_SIZE * MAP_ROW *  0);
    BuildTileOffsets(MAP_COL * 2, MAP_ROW * 1, TILE_SIZE * MAP_COL *  1, TILE_SIZE * MAP_ROW *  0);

    BuildTileOffsets(MAP_COL * 0, MAP_ROW * 2, TILE_SIZE * MAP_COL * -1, TILE_SIZE * MAP_ROW *  1);
    BuildTileOffsets(MAP_COL * 1, MAP_ROW * 2, TILE_SIZE * MAP_COL *  0, TILE_SIZE * MAP_ROW *  1);
    BuildTileOffsets(MAP_COL * 2, MAP_ROW * 2, TILE_SIZE * MAP_COL *  1, TILE_SIZE * MAP_ROW *  1);

    return imgCnt;
}

int PrintActor(int const id)
{
	const Actor *a = ActMan_GetActor(id)	;
	printf("actor x pos = %d",a->position.x);
    return 1;
}

// keep enemey on the road
// the sand down from the top of the screen faster
// than the traffic can go.  This gives the illusion
// of the hero is passing cars instead of going the
// wrong way down a one way road. The logic for staying
// on the road is:
// When we placed enemies on the road they were placed
// directly on a tile.  No offsets.  Look at the
// tile above (r - 1). If its sand look to the left of the sand tile.
// if its road move to the left.  if not, move to the right.
// do the same for the second tile (r - 2) If there is not a sand 
// tile head toward the preferedX position.  
int AvoidRoadBoundry(Actor *a)
{
	Grid2D myMapPos;	
	Position2D myWorldPos;
	Position2D myFrontCenter;
	myFrontCenter.x = a->position.x + TILE_SIZE / 2;
	myFrontCenter.y = a->position.y;

	// where in the world is myFrontCenter
	myWorldPos.x = myFrontCenter.x - x_offset;
	myWorldPos.y = myFrontCenter.y - y_offset;

	// let's anchor ourself in the center chunk
	while (myWorldPos.x < 0)        {myWorldPos.x += CHUNK_W;}
	while (myWorldPos.x >= CHUNK_W) {myWorldPos.x -= CHUNK_W;}
	while (myWorldPos.y < 0)        {myWorldPos.y += CHUNK_H;}
	while (myWorldPos.y >= CHUNK_H) {myWorldPos.y -= CHUNK_H;}

	// Now that we're in the center we're safe to look around.
	// let's get our map position in the 2d array 
	myMapPos.row = myWorldPos.y / TILE_SIZE;
	myMapPos.col = myWorldPos.x / TILE_SIZE;

	// we want to look at our current location and the 2 tiles 
	// above our current location.we can do this safely because
	// were looking relative to the center chunk.  We're certain 
	// there are at least MAP_ROW more rows above us.  
	// (currently 10 rows above us)

    int sandDetected = 0;
	// this was a loop used to give the little guy more warning of a road change.
	// But, in game it wasn't worth it.
	for (int i = 0; i < 1; i++)
	{
		int row = myMapPos.row - i;
		if (TileMap[row][myMapPos.col].mapkey == 'S' ||
		    TileMap[row][myMapPos.col].mapkey == 'G')
		{
			sandDetected = 1;
			int moveLeft = 1;

			if (a == Hero)
			{
				if (TileMap[row][myMapPos.col].mapkey == 'S')
				{
					// we ignore hero touches sand. Maybe one day we'll add a
					// particle effect for kicking up dust?  
					return 0; 
				}
			}

			if (TileMap[row][myMapPos.col].mapkey == 'G')
			{
				BlowUpActor(a);
				return 1;
			}

			for (int n = 0; n < 5; n++)
			{
				if (TileMap[row][myMapPos.col - n].mapkey == 'R')
				{
					moveLeft = 1;
					break;
				}
				else if (TileMap[row][myMapPos.col + n].mapkey == 'R')
				{
					moveLeft = 0;
					break;
				}
			}
			if (moveLeft)
			{
				a->position.x -= a->speed/2;	
				a->slideDirection = SLIDE_LEFT;
				a->slideTick = 15;
			}
			else
			{
			    a->position.x += a->speed/2;	
				a->slideDirection = SLIDE_RIGHT;
				a->slideTick = 15;
			}
		}
	}
	return sandDetected;
}

// reduction a value 0 - 9
// 9 reduces the size of the collision box by 10%
// 8 reduces the size of the collision box by 20%
// the pattern continues.  0 is no reduction.  
// if reduction is applied the x and y is also
// adjusted by 1/2 of the reduction.
int ActorOverlap(const Actor *a, const Actor *b, int reduction)
{
    Actor am;
    Actor bm;
    ActorClone(&am, a); //am == Actor a (m)odified for reduction values
    ActorClone(&bm, b); //bm == Actor a (m)odified for reduction values
    
    if (reduction > 9 || reduction < 0 ){reduction = 0;}
    if (reduction > 0)
    {
        int delAh = 0;
        int delAw = 0;
        int delBh = 0;
        int delBw = 0;
        float fred =reduction / 10.0f;
        am.size.w = (int)(am.size.w * fred);
        am.size.h = (int)(am.size.h * fred);
        bm.size.w = (int)(bm.size.w * fred);
        bm.size.h = (int)(bm.size.h * fred);
        
        delAh = (int)(a->size.h - (int)(a->size.h * fred)) / 2;
        delAw = (int)(a->size.w - (int)(a->size.w * fred)) / 2;
        delBh = (int)(b->size.h - (int)(b->size.h * fred)) / 2;
        delBw = (int)(b->size.w - (int)(b->size.w * fred)) / 2;
        am.position.x += delAw;
        am.position.y += delAh;
        bm.position.x += delBw;
        bm.position.y += delBh;
    }
    
	int bRight   = bm.position.x + bm.size.w;
	int bBottom  = bm.position.y + bm.size.h;
	int aRight  = am.position.x  + am.size.w;
	int aBottom = am.position.y  + am.size.h;

	if ( (aRight > bm.position.x) && (am.position.x < bRight) &&
		(aBottom > bm.position.y) && (am.position.y < bBottom) )
	{
		return 1;
	}
	return 0;
}

int AviodEnemies(Actor *a, Actor *enemies[], int eCnt)
{
	for (int i = 0; i < eCnt; i++)
	{
		Actor *b = enemies[i];
		if (b == a) {continue;}
		if (b->slideTick){continue;}// b is sliding... leave it alone
		if (ActorOverlap(a, enemies[i], 0))
		{
			if (a->slideTick > 0 || b->slideTick > 0)
			{
				//they are both sliding and out of control.  blow one up.
				if (b->slideTick > a->slideTick)
				{
					BlowUpActor(b);
				}
				else
				{
					BlowUpActor(a);	
				}
				return 1;
			}

			int moveALeft = a->position.x < b->position.x;
			int speedUp = a->position.y > b->position.y;
			if (moveALeft)
			{
				a->position.x -= a->speed;
				a->slideDirection = SLIDE_LEFT;
				a->slideTick = 15;
				b->slideDirection = SLIDE_RIGHT;
				b->slideTick = 5;
			}
			else
			{
				a->position.x += a->speed;
				a->slideDirection = SLIDE_RIGHT;
				a->slideTick = 15;
				b->slideDirection = SLIDE_LEFT;
				b->slideTick = 5;
			}

			if (speedUp)
			{
				a->position.y += b->speed + a->speed;
			}
			else
			{
				a->position.y -= b->speed + a->speed;
			}

			break;
		}
	}
	return 1;	
}

// query for a list of enemies from the actor manager
// increase the enemy.y value so that he moves down the screen. 
// the enemy.x and enemy.y value are relative to the screen NOT the world.
// if the enemy.y value is off screen respawn him to the top of the screen on a road tile.
// potentially change his speed for new behavior.
int EnemySim(void)
{
	int cursor = 0;
	int eCnt = 0;
	Actor *enemies[MAX_TRAFFIC];
	eCnt = ActMan_GetActiveKinds(&cursor, TRAFFIC_KINDS, 2, enemies, MAX_TRAFFIC); 

	for (int i = 0; i < eCnt; i++)
	{

		// move towards bottom of screen.
		enemies[i]->position.y += enemies[i]->speed;

		// Respawn 
		if (enemies[i]->position.y > WIN_H)
		{
            if (greenPowerUpExpire_ms > gs->now_ms && enemies[i]->kind == ACT_KIND_CIVILIAN) {goto end_respawn;}
			enemies[i]->position.y = 0 - TILE_SIZE;
			enemies[i]->speed = (rand() % 3) + 3;
			while (1){
				int startCol = (rand() % MAP_COL); // 0 - 9
				if ((TileMap[frameView.firstRow][startCol].mapkey == 'R' ||
					TileMap[frameView.firstRow][startCol].mapkey == '|' ) &&
					(TileMap[frameView.firstRow - 2][startCol].mapkey == 'R' ||
					TileMap[frameView.firstRow - 2][startCol].mapkey == '|' ))
				{
					enemies[i]->position.x = startCol * TILE_SIZE;
					enemies[i]->preferredX = startCol * TILE_SIZE;
					break;
				}
			}
		}
        end_respawn:
		if (enemies[i]->slideTick == 0)
		{
			enemies[i]->slideDirection = SLIDE_NONE;
			AvoidRoadBoundry(enemies[i]);
			AviodEnemies(enemies[i], enemies, eCnt);
		}
		else
		{
			if (enemies[i]->slideDirection == SLIDE_RIGHT)
			{
				enemies[i]->position.x += enemies[i]->speed;
			}
			else
			{
				enemies[i]->position.x -= enemies[i]->speed;
			}
			enemies[i]->slideTick -= 1;
		}
		
	}
    
    return 1;
}

int InactiveExplosion(void)
{
	for (int i = 0; i < MAX_EXPLOSIONS; i++)	
	{
		if (Explosions[i].active == 0){return i;}
	}

	return 0; // well... this isn't true... but it's true enough for now. 
}

int BlowUpActor(Actor *a)
{
    int ie = InactiveExplosion();
    Explosions[ie].origin.x = a->position.x;
    Explosions[ie].origin.y = a->position.y;
    Explosions[ie].active = 1;

	if (gs->run_mode != RUNMODE_DEMO)
	{
		Sfx_PlayHit();
	 }

	if (a == Hero)
	{
		HeroGameOver();	
	}
	else
	{
		a->position.y = WIN_H;	
	}
	return 1;	
}

int ShotCollisionSim(Actor *shot)
{
    int cursor = 0;
    Actor *enemies[MAX_TRAFFIC];
    //ActMan_GetActive(&cursor, ACT_KIND_ENEMY, enemies, totalEnemy); 
    int eCnt = ActMan_GetActiveKinds(&cursor, TRAFFIC_KINDS, 2, enemies, MAX_TRAFFIC); 
    
    for (int i = 0; i < eCnt; i++)	
    {
        if (ActorOverlap(shot, enemies[i],0))
        {
            if (enemies[i]->kind == ACT_KIND_CIVILIAN)
            {
                gs->run_stats.score -= 10;
            }
            else 
            {
                gs->run_stats.score  +=20;
            }
            shot->slideTick = 0;
			BlowUpActor(enemies[i]);

            break;
        }
    }
    return 1;
}
int ShotSim(void)
{
	if (shotCooler)
	{
		shotCooler -= 1;	
	}

	for (int i = 0; i < MAX_SHOTS; i++)
	{
		Actor *a = Shots[i];
		if (a->slideTick)	
		{
			a->position.y -= a->speed;
			a->slideTick -= 1;
			ShotCollisionSim(a);
		}
		else
		{
			a->position.x =	-500;
		}
	}	
    return 1;
}

int FireShot(void)
{
	if (shotCooler > 0){return 0;}
    if (Hero->state == ACT_ST_HIDDEN) {return 0;}
 	
	for (int i = 0; i < MAX_SHOTS; i++)
	{
		Actor *a = Shots[i];
		if (a->slideTick) {continue;}
		a->slideTick = SHOT_TICK;
		a->slideDirection = SLIDE_UP;
		a->position.y = Hero->position.y - TILE_SIZE;
		a->position.x = Hero->position.x ;
		Sfx_PlayShot();

		break;
	}
	shotCooler = FIRE_TICK;
	return 1;	
}

int ExplosionRender(void)
{
	float radius = 32.0;	
	static const XXColor palette[] = {
		  {255,255,255,255},
		  {255,161,0,255},
		  {230,41,55,255},
		  {253,249,0,255},
		  {130,130,130,255},
		  {200,200,200,255}
		};
	int paletteCount = sizeof(palette) / sizeof(palette[0]);
	int maxFrames = 15; 

	
	for (int i = 0; i < MAX_EXPLOSIONS; i++)
	{
        radius = 32;
		if (Explosions[i].active)
		{
			//draw the particles
			for (int j = 0; j < PARTICLE_COUNT; j++)
			{
				//pick a color	
				int rcol = (rand() % paletteCount);
				
				//adjust radius
				radius = radius - Explosions[i].frame / 2;
				if (radius < 5) {radius = 5;}	
				
				//set the offsets relative to the origin
				int x = Explosions[i].origin.x + Explosions[i].particles[j].x;
				int y = Explosions[i].origin.y + Explosions[i].particles[j].y;

				//and... splatter.
				XXDrawCircle(x, y, radius, palette[rcol]);
			}
			Explosions[i].frame += 1;
			if (Explosions[i].frame > maxFrames)
			{
				Explosions[i].frame = 0;
				Explosions[i].active = 0;
			}
		}
	} 
    return 1;
}

int LoadExplosions(void)
{
	for (int i = 0; i < MAX_EXPLOSIONS; i++)
	{
		Explosions[i].frame = 0;
		Explosions[i].active = 0;
		for (int j=0; j < PARTICLE_COUNT; j++)
		{
			// some point that inside but not near the edges of a 64,64 square
			Explosions[i].particles[j].x = (rand() % 32) + 16; 
			Explosions[i].particles[j].y = (rand() % 32) + 16;
		}
	}
    return 1;
}

int RenderPowerUpTimer(void)
{
    int space = 21;
    int r = 7;
    int y = 14;
    int x = 14;
    
    // draw some light gray dot where the colored ones will go.
    for (int i = 0; i < 6; i++)
    {
        XXDrawCircle(x, y, r, XX_LIGHTGRAY);
        x += space;        
    }
    
    // if we need colored dots draw them now.  
    // GreenPowerUp is activated.
    if (greenPowerUpExpire_ms < gs->now_ms){ return 0; }
    x = 14;
    int dotDuration = DURATION_GREEN_PWRUP / 6;
    long long timeLeft = greenPowerUpExpire_ms - gs->now_ms;
    for (int i = 0; i < 6; i++)
    {
        int tm = i * dotDuration;
        if (timeLeft > tm) 
        {
            XXDrawCircle(x, y, r, TimerColors[i]);
            x += space;
        }
    }
    return 1;
}

int HudRender(void)
{
	XXDrawRectangle(0, 0, WIN_W, 32, XXRGBA(128, 0, 128, 255));
	XXDrawRectangleOutline(0, 0, WIN_W, 32, 2, XX_BLACK);
	char buf[64];
	sprintf(buf, "SCORE: %d",  gs->run_stats.score );
	int fontSize = 20;
	int tw = XXMeasureText(buf, fontSize);
	XXDrawText(buf, (WIN_W - tw) / 2, 6, fontSize, XX_WHITE);	
    RenderPowerUpTimer();
    return 1;
}

int Init(void)
{
	if (doInit == 0) {return 1;}
    TimerColors[0] = XX_RED;
    TimerColors[1] = XX_RED;
    TimerColors[2] = XX_YELLOW;
    TimerColors[3] = XX_YELLOW;
    TimerColors[4] = XX_GREEN;
    TimerColors[5] = XX_GREEN;
    

	// we need to set the right number of civilian and enemy actors to active.
	// There are 4 levels of difficulty so we'll divide MAX_CIVILIAN and MAX_ENEMY 
	// by 4.

	int newMaxCiv   = (MAX_CIVILIAN / 4) * gs->difficulty;
	int newMaxEnemy = (MAX_ENEMY / 4) * gs->difficulty;
	int cnt = 0;
	int cursor = 0;
	Actor *a[MAX_TRAFFIC];

	if (newMaxCiv == 0) {newMaxCiv = MAX_CIVILIAN;}
	if (newMaxEnemy == 0) {newMaxEnemy = MAX_ENEMY;}

	// Set some to active and hide all the others.
	// move them all off screen for good measure.
	cnt = ActMan_GetAll(&cursor,ACT_KIND_ENEMY, a, MAX_TRAFFIC);
	for (int i = 0; i < cnt; i++)
	{
		if (i < newMaxEnemy)
		{
			a[i]->state = ACT_ST_ACTIVE;
		}
		else
		{
			a[i]->state = ACT_ST_INACTIVE;
		}
		a[i]->position.y = WIN_H + 1;
	}
	
	cursor = 0;
	cnt = ActMan_GetAll(&cursor,ACT_KIND_CIVILIAN, a, MAX_TRAFFIC);
	for (int i = 0; i < cnt; i++)
	{
		if (i < newMaxCiv)
		{
			a[i]->state = ACT_ST_ACTIVE;
		}
		else
		{
			a[i]->state = ACT_ST_INACTIVE;
		}
		a[i]->position.y = WIN_H + 1;
	}
	
    // we'll set the actor state to hidden and give a slideTick of 10.  This
    //   will cause the hero to spawn in a few ticks.
	Hero->state = ACT_ST_HIDDEN;
	Hero->slideTick = 10;

	// And reset the score for a new game run.
	gs->run_stats.score  = 0; 
    
    // set the powerup score triggers
    greenPowerUpExpire_ms = 0;
    greenPowerUpScore = INITIAL_GREEN_PWRUP;
    redPowerUpScore = INITIAL_RED_PWRUP;
    
    // make sure the doInit global is set to 0 so we don't keep doing this.
	doInit = 0; // global static var

	return 1;
	
}

int HeroGameOver(void)
{
	Sfx_PlayGameOver();	
    Hero->state = ACT_ST_HIDDEN;
	gs->run_mode = RUNMODE_DEMO;       
	gs->screen_id = SCREEN_SCORES; 
	doInit = 1;
    return 1;
}

int StartGreenPowerUp(void)
{
    if (GreenPowerUp->state == ACT_ST_HIDDEN) {return 0;}
    GreenPowerUp->position.y = GreenPowerUp->size.h * -1;
    GreenPowerUp->state = ACT_ST_HIDDEN;
    greenPowerUpExpire_ms = gs->now_ms + DURATION_GREEN_PWRUP;
    
    return 1;
}

int StartRedPowerUp(void)
{
    if (RedPowerUp->state == ACT_ST_HIDDEN) {return 0;}
    RedPowerUp->position.y = RedPowerUp->size.h * -1;
    RedPowerUp->state = ACT_ST_HIDDEN;
    int cursor = 0;
    Actor *enemies[MAX_TRAFFIC];
    int eCnt = ActMan_GetActive(&cursor, ACT_KIND_ENEMY, enemies, MAX_TRAFFIC); 
    printf("Blowing up %d red actors\n", eCnt);
    for (int i = 0; i < eCnt; i++)	
    {
        gs->run_stats.score  +=20;
        BlowUpActor(enemies[i]);
    }
    return 1;
}

int HeroSim(void)
{
    int cursor = 0;
    Actor *enemies[MAX_TRAFFIC];
    int eCnt = ActMan_GetActiveKinds(&cursor, TRAFFIC_KINDS, 2, enemies, MAX_TRAFFIC);
    
    if (Hero->state == ACT_ST_HIDDEN && Hero->slideTick > 0)
    {
        Hero->slideTick -= 1;
        return 1;
    }
    
    // respawn hero to a safe spot;
    if (Hero->state == ACT_ST_HIDDEN)
    {
        Hero->state = ACT_ST_VISIBLE;
        Hero->position.x = 600;
        Hero->position.y = 400; 
    }
    
    for (int i = 0; i < eCnt; i++)
    {
        if (ActorOverlap(Hero, enemies[i],9)) 
        {
            BlowUpActor(enemies[i]);
			BlowUpActor(Hero);
			break;
        }
    }
    
    if (GreenPowerUp->state == ACT_ST_VISIBLE && ActorOverlap(Hero, GreenPowerUp,9)) 
    {
        StartGreenPowerUp();
    }
    
    if (RedPowerUp->state == ACT_ST_VISIBLE && ActorOverlap(Hero, RedPowerUp,9))   
    {
        StartRedPowerUp();
    }

	AvoidRoadBoundry(Hero);
    
    return 1;
}

int WorldSim(void)
{
    int worldMatrixRows = MAP_ROW * 3;
    int worldMatrixCols = MAP_COL * 3;
    y_offset += mapSpeed; 

    // Wrap offsets to keep them bounded 
    while (x_offset >  CHUNK_W / 2) x_offset -= CHUNK_W;
    while (x_offset < -CHUNK_W / 2) x_offset += CHUNK_W;

    while (y_offset >  CHUNK_H / 2) y_offset -= CHUNK_H;
    while (y_offset < -CHUNK_H / 2) y_offset += CHUNK_H;

    int rowHit = 0;
    frameView.firstRow = -1;
    frameView.lastRow = -1;
    frameView.firstCol = -1;
    frameView.lastCol = -1;

    for (int r = 0; r < worldMatrixRows; r++) 
    {
        int drawY = TileMap[r][0].y + y_offset;

        if ((drawY + TILE_SIZE > -1) && (drawY < WIN_H)) 
        {
            if (rowHit == 0){frameView.firstRow = r;}
            frameView.lastRow = r ;            
            rowHit = 1;
            int colHit = 0;
            for (int c = 0; c < worldMatrixCols; c++) 
            {
                int drawX = TileMap[r][c].x + x_offset;
                if ((drawX + TILE_SIZE > -1) && (drawX < WIN_W)) 
                {
                    if (colHit == 0){frameView.firstCol = c;}
                    frameView.lastCol = c;
                    colHit = 1;

                    // Draw tile
                    TileMap[r][c].imgAsset.dest.x = drawX;
                    TileMap[r][c].imgAsset.dest.y = drawY;
                } 
                else if (colHit == 1) 
                {
                    // exit inner loop once we went past visible region
                    break;
                }
            }
        } 
        else if (rowHit == 1) 
        {
            // exit outer loop once we went past visible region
            break;
        }
    }
    
    if ( frameView.firstRow == -1 ||
    frameView.lastRow == -1 ||
    frameView.firstCol == -1 ||
    frameView.lastCol == -1 ) 
    {
        // something bad happened.  
        return 0;
    }
    
    return 1;
}

int PowerUpRender(Actor *pwr)
{
    if (pwr->state == ACT_ST_VISIBLE)
    {
        pwr->imgAsset.dest.x = pwr->position.x;
        pwr->imgAsset.dest.y = pwr->position.y;
        DrawAsset(&pwr->imgAsset);
        return 1;
    }
    return 0;
}

int GameRender(void)
{
    // draw the tiles.
    for (int r = frameView.firstRow; r <= frameView.lastRow; r++)
    {
        for (int c = frameView.firstCol; c <= frameView.lastCol; c++)
        {
            DrawAsset(&TileMap[r][c].imgAsset);
        }
    }
    
    // draw the enemies
    int cursor = 0;
    Actor *enemies[MAX_TRAFFIC];
    int eCnt = ActMan_GetActiveKinds(&cursor, TRAFFIC_KINDS, 2, enemies, MAX_TRAFFIC); 
    
    for (int i = 0; i < eCnt; i++)
    {
        enemies[i]->imgAsset.dest.x = enemies[i]->position.x;
        enemies[i]->imgAsset.dest.y = enemies[i]->position.y;
        DrawAsset(&enemies[i]->imgAsset);
    }

	if (gs->run_mode != RUNMODE_DEMO)
	{
	    // draw the shots
	    for (int i = 0; i < MAX_SHOTS; i++)
	    {
	        Shots[i]->imgAsset.dest.x = Shots[i]->position.x;
	        Shots[i]->imgAsset.dest.y = Shots[i]->position.y;
	        DrawAsset(&Shots[i]->imgAsset);
	    }
	    
	       
	    // Probably draw the hero
	    if (Hero->state != ACT_ST_HIDDEN)
	    {
	        Hero->imgAsset.dest.x = Hero->position.x;
	        Hero->imgAsset.dest.y = Hero->position.y;
	        DrawAsset(&Hero->imgAsset);
	    }
	}
    
    // draw the decoration
    ExplosionRender(); // This draws the explosion.
    HudRender();       // This draws the HUD
    PowerUpRender(GreenPowerUp);
    PowerUpRender(RedPowerUp);
    return 1;
}

int SpawnPowerUp(Actor *pwr)
{
    if (pwr->state == ACT_ST_HIDDEN)
    {
        pwr->position.y = pwr->size.h * -1;
        pwr->position.x = WIN_W / 2;
        pwr->state = ACT_ST_VISIBLE;
    }
    return 1;
}

int MovePowerUp(Actor *pwr)
{
    if (pwr->state == ACT_ST_HIDDEN) {return 0;}
    if (pwr->position.y > WIN_H)
    {
        pwr->state = ACT_ST_HIDDEN;
        return 0;
    }
    
    pwr->position.y += pwr->speed;
    return 1;
}

int PowerUpSim(void)
{
    if (gs->run_stats.score > greenPowerUpScore) 
    {
        greenPowerUpScore = redPowerUpScore + POWER_UP_INTERVAL;
        SpawnPowerUp(GreenPowerUp);
    }
    
    if (gs->run_stats.score > redPowerUpScore) 
    {
        redPowerUpScore = greenPowerUpScore + POWER_UP_INTERVAL;
        SpawnPowerUp(RedPowerUp);
    }
    
    MovePowerUp(GreenPowerUp);
    MovePowerUp(RedPowerUp);
    return 1;
}

int GameSim(GameState *gmst) 
{
	gs = gmst; // because I don't want to pass this around to every function that needs it.
	
    if (gs->run_mode != RUNMODE_PAUSED)
    {
        if (IsKBDown(KC_LEFT))  Hero->position.x -= Hero->speed;
        if (IsKBDown(KC_RIGHT)) Hero->position.x += Hero->speed;
        if (IsKBDown(KC_UP))    Hero->position.y -= Hero->speed;
        if (IsKBDown(KC_DOWN))  Hero->position.y += Hero->speed;

        if (IsKBDown(KC_TAB)) {FireShot();}

        /* clamp to screen */
        if (Hero->position.x < 0) Hero->position.x = 0;
        if (Hero->position.y < 0) Hero->position.y = 0;
        if (Hero->position.x > WIN_W - Hero->size.w) Hero->position.x = WIN_W - Hero->size.w;
        if (Hero->position.y > WIN_H - Hero->size.h) Hero->position.y = WIN_H - Hero->size.h;
        
        // TODO: I should be checking return values. 
        WorldSim();
        EnemySim();
		if (gs->run_mode != RUNMODE_DEMO)
		{
			Init();
			HeroSim();
 		    ShotSim();
		}
        PowerUpSim();
    }
    
    GameRender();
    return 1;
}
