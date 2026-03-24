// filename: game_sim.h
#ifndef GAME_SIM_H
#define GAME_SIM_H
#include "xx_types.h"

int LoadMap(const char *strMapFilePath);
int LoadMapTiles(void);
void PrintMap(void);
int LoadPlayer(void);
int LoadCivilian(void);
int LoadEnemy(void);
int LoadShots(void);
int LoadExplosions(void);
int LoadPowerUps(void);
int GameSim(GameState *gmst);
#endif
