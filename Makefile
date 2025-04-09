#----------------------------------------
# Makefile for Minesweeper
#----------------------------------------

bin_dir     = bin
src_dir     = src
lib_dir     = lib
logger_dir  = logger

cc          = clang
cc_flags    = -Wall -Wextra -g
cc_flags   += -framework Cocoa
cc_flags   += -I. -I$(lib_dir) -I$(logger_dir) -I$(src_dir)

# Source files
src_common  = $(src_dir)/canopy.m \
              $(src_dir)/minesweeper.c \
              $(src_dir)/canopy_event.c \
              $(src_dir)/canopy_time.c \
              $(src_dir)/common.c \
              $(src_dir)/bmp.c \
              $(src_dir)/picasso.c \
              $(logger_dir)/logger.c

# Game binary name
game        = minesweeper
outputs     = $(addprefix $(bin_dir)/, $(game))

# Default target
all: $(outputs)

# Link all source files into one binary
$(bin_dir)/%: $(src_common)
	@mkdir -p $(bin_dir)
	$(cc) $(cc_flags) $^ -o $@

# Clean rule
clean:
	rm -rf $(bin_dir)

.PHONY: all clean
