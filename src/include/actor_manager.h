/* Filename: src/include/actor_manager.h */
#ifndef ACTOR_MANAGER_H
#define ACTOR_MANAGER_H

#include "actor.h"

/* Lifecycle / access */
Actor* ActMan_Create(int kind);
Actor* ActMan_GetActor(int actorId);

/* Iteration (cursor = resume scan index; set *cursor=0 to start; *cursor=0 means done) */
int ActMan_GetActive(int *cursor, const int kind, Actor *out[], int outCap);
int ActMan_GetActiveKinds(int *cursor, const int kinds[], int kindCnt, Actor *out[], int outCap);
int ActMan_GetAllActive(int *cursor, Actor *out[], int outCap);

/* Deactivation */
int ActMan_DeactivateActiveKinds(const int kinds[], int kindCnt);
int ActMan_DeactivateActiveKind(const int kind);
int ActMan_DeactivateActive(const int actorId);

/* Returns actors of a single kind, regardless of active/inactive state */
int ActMan_GetAll(int *cursor, const int kind, Actor *out[], int outCap);

/* Returns actors of any of the given kinds, regardless of active/inactive state */
int ActMan_GetAllKinds(int *cursor, const int kinds[], int kindCnt, Actor *out[], int outCap);

#endif /* ACTOR_MANAGER_H */
