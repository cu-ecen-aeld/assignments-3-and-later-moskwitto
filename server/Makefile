# aesdsocket Makefile

CC      ?= gcc
CFLAGS  ?= -Wall -Werror -g
LDFLAGS ?=
LDLIBS  ?= -lpthread

OBJ := aesdsocket

all: $(OBJ)

$(OBJ): $(OBJ).c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(OBJ)
