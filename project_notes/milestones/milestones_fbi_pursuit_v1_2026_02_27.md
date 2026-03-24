# Filename: milestones_fbi_pursuit_v1_2026_02_27.md
# Date: 2026-02-27
# Author: Ronald Weidner (Xecronix) + Vega
# Purpose: Milestone recap for FBI Pursuit (C + raylib). Capture what is implemented in the current playable build.

-----------------------------------------------------------------------
HIGH-LEVEL MILESTONES
-----------------------------------------------------------------------

Milestone 1 - Boot to frame (COMPLETE)
- Window init + stable game loop (raylib)
- ClearBackground + draw calls per frame
- Clean shutdown and FreeAssets()

Milestone 2 - Tile world render (COMPLETE)
- level.map loaded (MAP_ROW x MAP_COL)
- tiles.png atlas loaded once
- tile "views" created via src.x offsets into shared texture id
- DrawAsset contract stabilized (dest is top-left)

Milestone 3 - Forever scroller foundation (COMPLETE)
- 3x3 NeighborhoodMap built from base chunk (tile x/y in world pixels)
- y_offset scroll (mapSpeed) creates illusion of forward motion
- offset wrap/recenter keeps offsets bounded (prevents drift)
- view culling: draw only visible tiles
- frameView bounds captured (first/last visible row/col) for convenience

Milestone 4 - Hero playable (COMPLETE)
- hero spawned and rendered
- movement via input
- clamp to screen bounds

Milestone 5 - Traffic + shooting + effects (COMPLETE / GREEN)
- ActorManager fixed pool in use (no malloc gameplay)
- Enemies spawned, move downscreen, respawn when off-screen
- Enemy spawn constrained to road tiles
- Enemy "stay on road" behavior via tile probe and lane drift
- Enemy-to-enemy avoidance via AABB overlap + slideTick commitment
- Shots: fixed pool (MAX_SHOTS), cooldown (shotCooler), fire action
- Shot sim: move, lifetime, recycle
- Shot vs enemy collision: hit resets enemy + deactivates shot
- Explosions: real-time particle effect (pool + frames + random palette)

-----------------------------------------------------------------------
IMPLEMENTED GAME CONCEPTS (CURRENT BUILD)
-----------------------------------------------------------------------

Core loop and rendering
- Update/draw loop
- Layered draw order: tiles -> enemies -> hero -> shots -> explosions

World + scrolling
- Tilemap loaded from file
- 3x3 chunk neighborhood (center chunk world origin at (0,0))
- Scrolling offsets and wrap logic for endless feel
- Visibility culling (draw only what is on-screen)

Actors + simulation
- Fixed pool actors (ActorManager)
- Enemies: spawn, move, avoid sand/grass boundary, avoid each other, respawn
- Hero: move + clamp

Combat
- Shots: pooled projectiles with cooldown + lifetime
- Collision: AABB overlap helper
- Shot hits enemy -> enemy reset + explosion

Effects / juice
- Explosion particle burst (fast/simple/looks good in motion)

Design patterns that emerged (happy accidents worth keeping)
- slideTick == commitment timer (busy until it expires)
  - used for enemy lane slides
  - reused for shot lifetime (100% commitment once fired)

-----------------------------------------------------------------------
KNOWN "SHAMELESS GREEN" / TECH DEBT (ACCEPTED FOR V1)
-----------------------------------------------------------------------
- Heavy globals (single-file prototype style)
- Some actor kinds reused/mislabeled (ok for now)
- frameView used as a convenience even though tile probe can replace most needs
- A few functions are doing too much (prototype speed > purity)

-----------------------------------------------------------------------
NEXT UP (IF CONTINUING THIS BUILD)
-----------------------------------------------------------------------
Minimum remaining gameplay
- Hero vs enemy collision (damage/lives/respawn behavior)
- Scoring rules (red car good, green car bad) if desired

Polish (optional)
- Flash on hit / hit pause / screen shake
- HUD (score, health, lives)
- Debug HUD (keys list, toggles, print/dump tools)
- Music + SFX

-----------------------------------------------------------------------
HANDOFF TO RORY (PLAYABLE BUILD NOTES)
-----------------------------------------------------------------------
What Rory can do right now
- Drive the hero, dodge traffic
- Fire shots (key mapped to avoid keyboard ghosting)
- Watch enemies pop with explosions

Kid knobs (easy tuning switches)
- mapSpeed
- MAX_ENEMY / MAX_CIVILIAN
- SHOT_TICK / FIRE_TICK
- enemy speed range

-----------------------------------------------------------------------
REBUILD INTENT (NEXT ENGINE PASS)
-----------------------------------------------------------------------
- Keep the working contracts:
  - DrawAsset contract (dest top-left)
  - Fixed pools and simple iteration
  - Tile probe for terrain rules
  - slideTick as commitment timer
- Rebuild with cleaner architecture:
  - GameState struct (reduce globals)
  - Input manager (raylib isolation)
  - Clear viewport + HUD separation (ViewportOrigin + ViewportSpace)
  - Split modules: map, sim, collision, rendering, ui