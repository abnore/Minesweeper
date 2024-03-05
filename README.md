# Minesweeper
Making minesweeper like the old Win98/2000 style using SDL2 and C.
This is a small personal project to learn C.

<img src="/img/minedemo.jpg" alt="Example showing the graphics" width="300">

## In progress:

* Making it varying size, currently 16x16 grid
* Maybe incorporating sounds
* Getting the question mark tiles

### When running
It now shows the the number of mines correctly, including negative numbers.
The timer is functioning and will stop when the game is over. Graphics work like they should.
If you press a mine, and move the cursor it will let you undo the press.
```console
make
```
The macro `BOMB_CHANCE` determines the probability of a cell being a bomb. The lower this number,
The more bombs you will get.

```console
make run
```
This will build and start the program.

**Made for mac, not cross-platform**
