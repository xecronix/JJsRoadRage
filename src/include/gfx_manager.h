// filename: gfx_manager.h
#ifndef GFX_MANAGER_H
#define GFX_MANAGER_H

#include "xx_types.h"

int LoadAssetFromFile(ImageAsset *a, const char *fname);
int DrawAsset(const ImageAsset *a);
int FreeAssets(void);
void XXDrawCircle(int x, int y, float radius, XXColor c);
void XXDrawRectangle(int x, int y, int w, int h, XXColor c);
void XXDrawRectangleOutline(int x, int y, int w, int h, int thickness, XXColor c);

void XXDrawText(const char *text, int x, int y, int fontSize, XXColor c);
int  XXMeasureText(const char *text, int fontSize);

int  Gfx_InitGameRT(int w, int h);
int  Gfx_IsGameRTReady(void);
void Gfx_BeginGameRT(XXColor clear);
void Gfx_EndGameRT(void);
void Gfx_PresentGameRT(int dstX, int dstY, int dstW, int dstH);

#endif
