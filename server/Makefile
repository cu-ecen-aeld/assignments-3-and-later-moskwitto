# aesdsocket Makefile

CROSS_COMPILE ?=
CC            ?= $(CROSS_COMPILE)gcc
CFLAGS        += -g -Wall -Werror
LDFLAGS       += -lpthread

OBJ := aesdsocket

all: $(OBJ)

$(OBJ): $(OBJ).c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ)
