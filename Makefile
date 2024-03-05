GCC := gcc
BIN := bin
SRC := src
CFLAGS := -pedantic -Wall -Wextra -std=gnu11
CFLAGS += -I/Library/Frameworks/SDL2.framework/Headers
CFLAGS += -I/Library/Frameworks/SDL2_image.framework/Headers
LDFLAGS := -F/Library/Frameworks -framework SDL2 -framework SDL2_image
LDFLAGS += -Wl,-rpath,/Library/Frameworks


$(BIN)/minesweeper:
	$(info Building and outputting to $(BIN) directory)
	@mkdir -p $(BIN)
	@$(GCC) $(CFLAGS) $(SRC)/minesweeper.c $(LDFLAGS) -o $@

clean:
	@rm -rf $(BIN)/minesweeper
	@rm -rf $(BIN)

run: $(BIN)/minesweeper
	./$(BIN)/minesweeper

.PHONY: clean run



