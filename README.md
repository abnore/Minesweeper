# Minesweeper
Making minesweeper like the old Win98/2000 style using SDL2 and C.
This is a small personal project to learn C.

<img src="/img/minedemo.jpg" alt="Example showing the graphics" width="300">

**Made for mac, not cross-platform**

## In progress:

* Making it varying size, currently 16x16 grid
* Maybe incorporating sounds

### When running
It now shows the the number of mines correctly, including negative numbers.
The timer is functioning and will stop when the game is over. Graphics work like they should.
If you press a mine, and move the cursor it will let you undo the press.
Additionaly, I added the question mark functionality so you can mark a tile a question mark, but it
is still pressable.

The macro `BOMB_CHANCE` determines the probability of a cell being a bomb. The lower this number,
the more bombs you will get. It is now set at a medium difficulty with 7. Good luck with anything
under 5.

The makefile is included, and will build to a bin file with the command
```console
make
```

If you want, this command will build and start the program.
```console
make run
```
