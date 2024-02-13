CC = gcc
LD = gcc

SRC = src
BIN = bin
LIB = lib
INC = include

LFLAGS  = -Wall -std=c99 
LFLAGS += -I$(LIB) 
LFLAGS += -arch arm64 
LFLAGS += -I$(INC)
LFLAGS += -I/opt/homebrew/Cellar/sdl2/2.28.5/include/SDL2  
LFLAGS += -L/opt/homebrew/Cellar/sdl2/2.28.5/lib
LFLAGS += -lSDL2

build:
	$(CC) $(LFLAGS) $(SRC)/main.c $(SRC)/graphics.c -o $(BIN)/mine
run:
	./bin/mine

clean:
	rm bin/*

all: build run
