# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

L5 is a Godot 4.6 game (working title "L5") targeting Linux, Windows, and Web. The dev environment is managed via Nix flakes — enter it with `nix develop` or `direnv allow`.

## Common Commands

All task runner commands use [go-task](https://taskfile.dev):

```sh
task godot-start      # Open Godot editor with this project
task super-lint       # Run GitHub super-linter via Docker
task super-format     # Run super-linter with auto-fix enabled
task update-flake-inputs  # Update all Nix flake inputs
```

Export targets are configured for Linux (`build/linux/`), Windows (`build/windows/`), and Web (`build/web/`). Export from the Godot editor: Project → Export.

## GDScript Linting

`gdtoolkit_4` is available in the Nix shell. Run the linter directly:

```sh
gdlint path/to/script.gd
gdformat path/to/script.gd
```

## Helix Editor Integration

The project configures Helix to use the Godot LSP over a local socket. The Godot editor must be running first (it opens the LSP on port 6005), then Helix will get autocompletion and error checking for GDScript files.

## Architecture

- **Godot 4.6**, Forward Plus renderer, 640×360 viewport with integer scaling.
- **SharedSounds** is a global autoload scene (`SharedSounds.tscn`) — use it for audio that should persist across scenes.
- `art/` holds sprites/assets; `build/` is gitignored (export output only).
- The main scene is set by UID in `project.godot`; find it via the Godot editor's filesystem dock.
- `ToggleDebug` is a defined input action (F11 by default) available in all scripts.

## Game Design Context

L5 is a hard sci-fi 2D factory/logistics game set in Jupiter's L5 Trojan asteroid swarm. Core reference: Satisfactory's logistics loop, but in space with real orbital mechanics. Key design constraints to keep in mind:

**World**
- Persistent MMORPG-style shared world; asteroid orbits match real ephemeris data (set N years in the future) and evolve in real time at game time scale (~10x compression)
- Asteroids average ~10 light-seconds apart; light-speed delay is a soft distance limiter
- Full asteroid map is known at game start; composition must be scanned within range
- No enemies — conflict is economic (land rights, patents, market prices)

**Progression arc**: personal ship → factory bay → permanent space station → asteroid claims → mining drones → space tug logistics network

**Asteroid types** (based on real Jupiter Trojan spectra): D-type (carbon/organics/volatiles, dominant), C-type (carbonaceous, possible water ice), P-type (silicate/metal mix). Volatiles require special extraction equipment.

**Logistics**: Space tugs replace conveyors. Propulsion tiers: solar sail → ion drive → cold gas → chemical → nuclear fission. Each trades speed vs. fuel vs. cargo capacity.

**Tech tree**: Patent purchases with credits (no component gating). Some recipes additionally unlock after producing a threshold quantity of a material.

**Economy**: Sell processed goods to Ganymede (primary sink) or to other players. Land rights purchased with credits.

**Death**: Ship destruction → respawn at last-used factory station as a backup intelligence upload.

**Factory stations**: 2D with multiple vertical floors; auto-layout tools available from the start; all merger/splitter tools unlocked immediately (logistics complexity lives in orbital mechanics, not tool gating).
