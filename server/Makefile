CROSS_COMPILE ?=
CC            ?= $(CROSS_COMPILE)gcc
CFLAGS        ?= -g -Wall -Werror
LDFLAGS       ?=
OBJ           := aesdsocket

all: $(OBJ)

$(OBJ): $(OBJ).c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) -lpthread

clean:
	rm -f $(OBJ)
