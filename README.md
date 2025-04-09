# Minesweeper

> **v2.0.0**  
> No SDL, no Raylib — this version is built **from scratch** using only **libc and Cocoa**.

A nostalgic Win98/2000-style **Minesweeper clone**, written in **pure C**, with no external game engine dependencies.

>[!WARNING]
>
>  **Made for macOS — not cross-platform.**

<img src="/img/minedemo.jpg" alt="Example showing the graphics" width="300">

---

## In Progress

- [x] Working 16x16 grid
- [ ] Variable grid size (WIP)
- [ ] Sounds?

---

## Features

- Accurate bomb counter — supports negative numbers
- Timer stops when the game ends
- Undo press: if you click and move away, the tile won’t stay pressed
- Right-click cycles through: flag → question mark → blank
- Question marks are **pressable**
- All graphics rendered using [`picasso`](https://github.com/abnore/picasso), [`canopy`](https://github.com/abnore/canopy), and [`blackbox`](https://github.com/abnore/blackbox)

---

## Gameplay

The macro `BOMB_CHANCE` controls difficulty.  
Lower = more bombs.  
Default is `7`, which is medium difficulty.  
Try under `5` for a real challenge.

---

## Building

### To build:
```bash
make
```

### To build & run:
```bash
make run
```

Build output goes to the bin/ directory.
