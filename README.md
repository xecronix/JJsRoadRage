# JJ's Road Rage: The Musical

JJ's Road Rage: The Musical is a small C + raylib arcade driving game built with Pelles C on Windows.

The current build is a one-level, endless-road score attack game with:
- keyboard controls
- a start screen
- a settings screen
- a pause screen
- a high score screen
- an in-dash stereo in fullscreen mode
- 6 music tracks
- 2 power-ups
- local high score saving


## What kind of game is this?

You drive on an endless scrolling road, dodge traffic, shoot red enemy cars, avoid hitting civilians, grab power-ups, and chase a higher score.

If you like arcade games, you'll like this game for sure.

## Current rules

### Goal
- Score as many points as possible before crashing.
- High scores are saved locally.

### Scoring
- Shoot a red enemy car: +20 points
- Shoot a green civilian car: -10 points

### Death
- If the hero collides with traffic, the hero blows up and the run ends.
- Going into grass is deadly.
- After game over, the game returns to the high score screen.

### Power-ups
There are currently 2 power-ups:

- Green power-up
  - Gives a timed effect.
  - A HUD timer is shown while the effect is active.
  - While active, civilian respawns are suppressed which allows clear shots at the red cars.

- Red power-up
  - Blows up all active enemy cars when collected.
  - Awards normal enemy score for each enemy destroyed by the effect.

### Music / dashboard
- In fullscreen mode, the dashboard stereo is shown below the playfield.
- The stereo displays the current song title.
- Long song names scroll inside the radio display.
- Volume and song selection can be controlled from the keyboard. 

## Controls

### During gameplay
- Arrow keys: move hero
- TAB: shoot
- P: pause
- ESC: pause

### Pause screen
- R: resume
- Q: quit

### Start screen
- TAB: start game
- S: open settings
- ESC: opens the pause/quit flow

### Settings screen
- 1 or keypad 1: Wimp
- 2 or keypad 2: Easy
- 3 or keypad 3: Hard
- 4 or keypad 4: Boss
- F: fullscreen
- W: windowed mode
- BACKSPACE: return to start screen

### High score screen
- BACKSPACE: return to start screen

### Stereo controls
These are active during normal play and most screens.

- 1 through 6: choose song
- + or keypad +: volume up
- - or keypad -: volume down

Note: on the Settings screen, keys 1 through 4 are used for difficulty selection, so song hotkeys are intentionally ignored there.

## Settings

The game currently supports these settings:

### Difficulty
There are 4 difficulty levels:
- 1 = Wimp
- 2 = Easy
- 3 = Hard
- 4 = Boss

Difficulty changes how many enemy and civilian actors are active during a run.

### Screen view
There are 2 screen modes:
- F = Fullscreen
- W = Window

Fullscreen mode also enables the dashboard stereo below the main playfield.

## Build instructions

This project is set up for Windows and Pelles C.

### Option 1: Build in the Pelles C IDE
Recommended for normal contributors.

1. Open `JJsRoadRage.ppj` in Pelles C.
2. Build the project in Debug or Release mode.
3. Run the generated executable from the project folder.

Important notes:
- Runtime assets are expected to be found relative to the executable.
- `raylib.dll` needs to be next to the executable at runtime.

### Option 2: Build with the batch file
This is the faster local workflow used during development.

Available commands:

```bat
project.bat build_debug
project.bat build_release
project.bat rebuild_debug
project.bat rebuild_release
project.bat run
project.bat clean
project.bat crude
project.bat diff
````

Notes:

* `project.bat` expects a working Pelles C command-line environment.
* `env_pelles32.bat` is used to set that environment.
* `crude` is a local code management snapshot tool ( https://github.com/xecronix/crude )
* `diff` depends on WinMerge ( https://github.com/winmerge/winmerge or https://winmerge.org/ )

## Project layout

```text
main.c                  Entry point
src/                    Game source
src/include/            Project headers
assets/                 Game art, audio, map, and scores
deps/include/           Third-party headers
deps/lib/               Third-party libraries
RaylibWin32.ppj         Pelles C project file
project.bat             Command-line build helper
raylib.dll              Runtime DLL
```

## Runtime files

The game expects assets to exist relative to the executable.

Important runtime files include:

* `assets/level1/...`
* `assets/scores`
* `raylib.dll`

If you move the executable, keep the asset folder and DLL with it.

## High scores

High scores are stored in:

```text
assets/scores
```

The file is created automatically if it does not already exist.

## Notes for contributors

* This is a small C project with a simple fixed-pool actor system.
* The game favors straightforward code over heavy abstraction.
* If you rename the executable/project output, update both the Pelles C project and any helper scripts you care about.

## Status

This build represents a playable release candidate / early finished version of the game.

It already includes:

* endless road gameplay
* traffic AI
* shooting
* scoring
* explosions
* high scores
* settings
* fullscreen dashboard stereo
* music selection
* power-ups
* HUD power-up timer

Future cleanup and refactoring may still happen, but the game is already playable as its own thing.

## Credits

* Built by Ronald Weidner (Xecronix).
* Art: Ronald Weidner
* Music: Pixabay Music - https://pixabay.com/music/
* Code Review + Debug Partner: ChatGPT

Powered by:

* C
* Pelles C
* raylib
* Paint.NET
