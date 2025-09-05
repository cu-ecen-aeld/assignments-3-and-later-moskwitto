# aesdsocket Makefile

CC      =aarch64-none-linux-gnu-gcc
CFLAGS  ?= -Wall -Werror -g
LDFLAGS ?=
LDLIBS  ?= -lpthread

OBJ := aesdsocket

all: $(OBJ)

$(OBJ): $(OBJ).c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(OBJ)
