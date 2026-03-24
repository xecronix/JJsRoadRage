/* Filename: src/actor_manager.c */
#include "xx_conf.h"
#include "actor_manager.h"
#include "actor_templates.h"   /* expects TPL_* and ACT_KIND_* values */
#include <string.h>            /* memset */

/* Storage */
static Actor _actors[MAX_ACTOR];
static int _actorCnt = 0; /* highest slot ever touched (0..MAX_ACTOR) */

/* Assumes these exist in actor.h:
   - ACT_ST_ACTIVE
   - ACT_ST_INACTIVE
*/
static int _IsActive(const Actor *a)
{
    return a && (a->state == ACT_ST_ACTIVE);
}

static int _IsRecyclable(const Actor *a)
{
    return a && (a->recyclable);
}

static void _Deactivate(Actor *a)
{
    if (!a) return;
    a->state = ACT_ST_INACTIVE;
}

/* Map kind -> template.
   Add cases as you create templates. Return NULL for unknown kinds. */
static const Actor* _TemplateForKind(int kind)
{
    switch (kind) {
        case ACT_KIND_HERO:      return &TPL_HERO;
        case ACT_KIND_ENEMY:     return &TPL_ENEMY;
		case ACT_KIND_CIVILIAN:     return &TPL_CIVILIAN;
        case ACT_KIND_HEROSHOT:  return &TPL_HEROSHOT;	
        case ACT_KIND_POWER_UP:  return &TPL_POWER_UP;		

        default: return 0;
    }
}

static int _KindInList(int kind, const int kinds[], int kindCnt)
{
    if (!kinds || kindCnt <= 0) return 0;
    for (int i = 0; i < kindCnt; i++) {
        if (kinds[i] == kind) return 1;
    }
    return 0;
}

Actor* ActMan_GetActor(int actorId)
{
    if (actorId < 0 || actorId >= MAX_ACTOR) return 0;
    return &_actors[actorId];
}

Actor* ActMan_Create(int kind)
{
    const Actor *tpl = _TemplateForKind(kind);
    if (!tpl) return 0;

    /* Find a free slot (inactive). Scan only up to _actorCnt first, then extend. */
    int i;

    for (i = 0; i < _actorCnt; i++) {
        if (_IsRecyclable(&_actors[i])) {
            _actors[i] = *tpl;
            return &_actors[i];
        }
    }

    if (_actorCnt >= MAX_ACTOR) return 0;

    /* Use next unused slot */
    i = _actorCnt++;
    _actors[i] = *tpl;
    return &_actors[i];
}

/* Core scan helper */
static int _GetActiveScan(int *cursor, int filterMode, const int kinds[], int kindCnt, int singleKind,
                          Actor *out[], int outCap)
{
    if (!out || outCap <= 0) {
        if (cursor) *cursor = 0;
        return 0;
    }

    int start = 0;
    if (cursor && *cursor > 0) start = *cursor;

    int n = 0;

    for (int i = start; i < _actorCnt && i < MAX_ACTOR; i++) {
        Actor *a = &_actors[i];
        if (!_IsActive(a)) continue;

        int ok = 1;
        if (filterMode == 1) {
            ok = (a->kind == singleKind);
        } else if (filterMode == 2) {
            ok = _KindInList(a->kind, kinds, kindCnt);
        } else {
            ok = 1; /* all active */
        }

        if (!ok) continue;

        out[n++] = a;
        if (n >= outCap) {
            /* resume scanning at next index */
            if (cursor) {
                int next = i + 1;
                *cursor = (next >= _actorCnt) ? 0 : next;
            }
            return n;
        }
    }

    /* reached end */
    if (cursor) *cursor = 0;
    return n;
}

int ActMan_GetActive(int *cursor, const int kind, Actor *out[], int outCap)
{
    return _GetActiveScan(cursor, 1, 0, 0, kind, out, outCap);
}

int ActMan_GetActiveKinds(int *cursor, const int kinds[], int kindCnt, Actor *out[], int outCap)
{
    return _GetActiveScan(cursor, 2, kinds, kindCnt, 0, out, outCap);
}

int ActMan_GetAllActive(int *cursor, Actor *out[], int outCap)
{
    return _GetActiveScan(cursor, 0, 0, 0, 0, out, outCap);
}

int ActMan_DeactivateActive(const int actorId)
{
    if (actorId < 0 || actorId >= _actorCnt || actorId >= MAX_ACTOR) return 0;
    Actor *a = &_actors[actorId];
    if (!_IsActive(a)) return 0;
    _Deactivate(a);
    return 1;
}

int ActMan_DeactivateActiveKind(const int kind)
{
    int n = 0;
    for (int i = 0; i < _actorCnt; i++) {
        Actor *a = &_actors[i];
        if (_IsActive(a) && a->kind == kind) {
            _Deactivate(a);
            n++;
        }
    }
    return n;
}

int ActMan_DeactivateActiveKinds(const int kinds[], int kindCnt)
{
    if (!kinds || kindCnt <= 0) return 0;

    int n = 0;
    for (int i = 0; i < _actorCnt; i++) {
        Actor *a = &_actors[i];
        if (_IsActive(a) && _KindInList(a->kind, kinds, kindCnt)) {
            _Deactivate(a);
            n++;
        }
    }
    return n;
}

/* Filename: src/actor_manager.c */

static int _GetAnyScan(int *cursor, int filterMode, const int kinds[], int kindCnt, int singleKind,
                       Actor *out[], int outCap)
{
    if (!out || outCap <= 0) {
        if (cursor) *cursor = 0;
        return 0;
    }

    int start = 0;
    if (cursor && *cursor > 0) start = *cursor;

    int n = 0;

    for (int i = start; i < _actorCnt && i < MAX_ACTOR; i++) {
        Actor *a = &_actors[i];

        /* NOTE: no active check here. We include active + inactive. */

        int ok = 1;
        if (filterMode == 1) {
            ok = (a->kind == singleKind);
        } else if (filterMode == 2) {
            ok = _KindInList(a->kind, kinds, kindCnt);
        } else {
            ok = 1; /* all touched slots */
        }

        if (!ok) continue;

        out[n++] = a;
        if (n >= outCap) {
            if (cursor) {
                int next = i + 1;
                *cursor = (next >= _actorCnt) ? 0 : next;
            }
            return n;
        }
    }

    if (cursor) *cursor = 0;
    return n;
}

int ActMan_GetAll(int *cursor, const int kind, Actor *out[], int outCap)
{
    return _GetAnyScan(cursor, 1, 0, 0, kind, out, outCap);
}

int ActMan_GetAllKinds(int *cursor, const int kinds[], int kindCnt, Actor *out[], int outCap)
{
    return _GetAnyScan(cursor, 2, kinds, kindCnt, 0, out, outCap);
}
