/* file: gfx_manager.c */
#include "raylib.h"
#include "xx_types.h"
#include "gfx_manager.h"
#include <stdio.h>
#include <string.h> /* memset */

#define AM_MAX_IMAGES 128

static int _nextIndex = 0;
static Texture2D _images[AM_MAX_IMAGES];

/* Render target (game viewport). Must be created AFTER InitWindow(). */
static RenderTexture2D _gameRT;
static int _gameRT_ready = 0;

static Color ToRl(XXColor c)
{
    Color r = { c.r, c.g, c.b, c.a };
    return r;
}

/* Call once after InitWindow() */
int Gfx_InitGameRT(int w, int h)
{
    if (_gameRT_ready) return 1;

    _gameRT = LoadRenderTexture(w, h);

    /* raylib sets ids to 0 on failure */
    _gameRT_ready = (_gameRT.id != 0 && _gameRT.texture.id != 0);
    if (!_gameRT_ready) {
        printf("ERROR: LoadRenderTexture(%d,%d) failed\n", w, h);
        memset(&_gameRT, 0, sizeof(_gameRT));
        return 0;
    }

    return 1;
}

int Gfx_IsGameRTReady(void)
{
    return _gameRT_ready;
}

/* Begin drawing into the game render texture */
void Gfx_BeginGameRT(XXColor clear)
{
    if (!_gameRT_ready) return;
    BeginTextureMode(_gameRT);
    ClearBackground(ToRl(clear));
}

/* End drawing into the game render texture */
void Gfx_EndGameRT(void)
{
    if (!_gameRT_ready) return;
    EndTextureMode();
}

/* Draw the game render texture onto the real screen.
   Call this AFTER BeginDrawing(), and ONLY when NOT in BeginTextureMode(). */
void Gfx_PresentGameRT(int dstX, int dstY, int dstW, int dstH)
{
    if (!_gameRT_ready) return;

    /* RenderTexture is flipped vertically in raylib when sampling */
    Rectangle src = { 0.0f, 0.0f, (float)_gameRT.texture.width, -(float)_gameRT.texture.height };
    Rectangle dst = { (float)dstX, (float)dstY, (float)dstW, (float)dstH };

    DrawTexturePro(_gameRT.texture, src, dst, (Vector2){0,0}, 0.0f, WHITE);
}

int LoadAssetFromFile(ImageAsset *a, const char *fname)
{
    if (!a || !fname) return 0;
    if (_nextIndex >= AM_MAX_IMAGES) return 0;

    _images[_nextIndex] = LoadTexture(fname);
    if (_images[_nextIndex].id == 0) {
        printf("ERROR: failed to load %s\n", fname);
        return 0;
    }

    a->id = _nextIndex;
    a->ready = 1;
    _nextIndex++;
    return _nextIndex;
}

int FreeAssets(void)
{
    for (int i = 0; i < _nextIndex; i++) {
        if (_images[i].id != 0) {
            UnloadTexture(_images[i]);
            _images[i].id = 0;
        }
    }
    _nextIndex = 0;

    if (_gameRT_ready) {
        UnloadRenderTexture(_gameRT);
        memset(&_gameRT, 0, sizeof(_gameRT));
        _gameRT_ready = 0;
    }

    return 1;
}

int DrawAsset(const ImageAsset *a)
{
    if (!a) return 0;
    if (a->id < 0 || a->id >= _nextIndex) return 0;

    Rectangle src;
    Rectangle dst;
    Vector2 origin;

    src.x = (float)a->src.x;
    src.y = (float)a->src.y;
    src.width  = (float)a->imgSize.w;
    src.height = (float)a->imgSize.h;

    origin.x = (float)a->rotationPoint.x;
    origin.y = (float)a->rotationPoint.y;

    /* Keep API: a->dest is TOP-LEFT. */
    dst.x = (float)a->dest.x + origin.x;
    dst.y = (float)a->dest.y + origin.y;
    dst.width  = (float)a->imgSize.w;
    dst.height = (float)a->imgSize.h;

    DrawTexturePro(_images[a->id], src, dst, origin, a->rotationAngle, WHITE);
    return 1;
}

/* Wrapper primitives (draw to whichever target is currently active) */

void XXDrawCircle(int x, int y, float radius, XXColor c)
{
    DrawCircle(x, y, radius, ToRl(c));
}

void XXDrawRectangle(int x, int y, int w, int h, XXColor c)
{
    DrawRectangle(x, y, w, h, ToRl(c));
}

void XXDrawRectangleOutline(int x, int y, int w, int h, int thickness, XXColor c)
{
    DrawRectangleLinesEx((Rectangle){ (float)x, (float)y, (float)w, (float)h },
                         (float)thickness, ToRl(c));
}

void XXDrawText(const char *text, int x, int y, int fontSize, XXColor c)
{
    DrawText(text, x, y, fontSize, ToRl(c));
}

int XXMeasureText(const char *text, int fontSize)
{
    return MeasureText(text, fontSize);
}
