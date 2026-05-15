# L5

A hard sci-fi 2D space factory game set in Jupiter's L5 Lagrange point Trojan asteroid swarm. You play as an uploaded intelligence building a space mining corporation — think Satisfactory, but in space, with real orbital mechanics.

## Concept

The L5 Trojan asteroids sit ~60° behind Jupiter in its orbit, ~5.2 AU from the Sun. Over 5,600 known asteroids populate this swarm, averaging about 10 light-seconds apart. The game runs at ~10x time compression so interasteroid travel is meaningful but not punishing.

You start with a small personal spacecraft, manually mine unclaimed asteroids, and gradually build up factory stations, mining drones, and a logistics network of space tugs. Processed goods are sold to Ganymede (the trade hub). The endgame is a sprawling automated supply chain spanning dozens of asteroids.

## Core Pillars

- **Hard sci-fi aesthetics** — real orbital mechanics, light-speed as a distance limiter, asteroid compositions based on actual D/P/C-type spectral classes
- **Persistent MMORPG world** — shared universe with real-time orbital evolution, set N years in the future from a fixed epoch
- **Factory automation** — same satisfying logistics loop as Satisfactory but 2D, streamlined, and space-native
- **No enemies** — conflict replaced by economics: land rights, patent licensing, and market competition

## Progression

1. **Personal ship phase** — spawn at a random point, fly around, manually mine small unclaimed asteroids, process ore in your ship's tiny onboard factory; sell to Ganymede for credits
2. **Factory bay phase** — build your first deployable factory bay (solar-powered); unlock more processing recipes; still manually hauling cargo
3. **Permanent station phase** — establish a fixed space station factory on a claimed asteroid; vastly more throughput but immobile (or very costly to relocate)
4. **Claim & expand** — use credits to buy land rights on additional asteroids; deploy mining drones that produce autonomously
5. **Logistics network** — replace manual hauling with space tugs running orbital transfer routes (like vehicles in Satisfactory); optimize tug propulsion tiers for speed vs. capacity

## Key Systems

### Space Tugs (logistics)
Progression of propulsion tech, each a tradeoff of speed / fuel / capacity:
- Solar sail
- Ion drive
- Pressurized gas (cold gas thrusters)
- Chemical thrusters
- Nuclear fission

### Asteroid Types
Based on real Jupiter Trojan composition:
- **D-type** (dominant) — dark, carbon/organic-rich, volatile-bearing; require special equipment for volatile extraction
- **C-type** — carbonaceous, water ice possible
- **P-type** — similar to D-type but with silicate signatures; metal/silicate mix

Full asteroid map is revealed from the start; composition (resource yield) must be scanned from within range.

### Tech Tree
Unlocked by purchasing patents with credits — no gating behind crafting components. Some recipes additionally unlock after producing a threshold quantity of a specific material.

### Power
Factory stations run on solar (weak at 5.2 AU), upgradeable to RTG / nuclear fission. Power vs. fuel vs. material is a meaningful tradeoff throughout.

### Death & Respawn
Destroy your ship at the last second → backup intelligence restores at your last-used factory station. Permanent stations serve as respawn anchors.

## Multiplayer
- Persistent shared world; other players occupy the same asteroid swarm
- Player-to-player trade in addition to Ganymede market
- Land rights create soft territorial boundaries rather than PvP

## Setting
The game is set a fixed number of years in the future. Asteroid orbits match real ephemeris data for that epoch and evolve in real time (at game time scale). The Lucy mission's targets (Patroclus, Eurybates, Leucus, Polymele, Orus, Donaldjohanson) are in-universe landmarks.

Notable real asteroids in the L5 swarm:
- **617 Patroclus** (~140 km, binary) — density ~0.8 g/cm³, likely water ice interior
- **1172 Äneas** (~118 km)
- **1867 Deiphobus** (~118 km)

## Art Direction
- 2D, multiple vertical floors within factory stations
- Conveys the vastness of space through travel time and the sparse star field, not just visual scale

## Workflows

### Development
```sh
nix develop          # enter dev shell (or: direnv allow)
```

### Lint / Format
```sh
gdlint path/to/script.gd
gdformat path/to/script.gd
task super-lint      # full repo lint via Docker
task super-format    # auto-fix
```
