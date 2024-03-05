GCC=gcc
BIN=bin
SRC=src
CFLAGS= -pedantic -Wall -Wextra -std=c99 -F/Library/Frameworks -framework SDL2 -framework SDL2_image -I/Library/Frameworks/SDL2.framework/Headers -I/Library/Frameworks/SDL2_image.framework/Headers -rpath /Library/Frameworks

$(BIN)/main: $(SRC)/minesweeper.c
	$(GCC) $(CFLAGS) -o $@ $^

$(BIN):
	mkdir -p $(BIN)

clean: $(BIN)/main
	rm -rf main

.PHONY: clean
