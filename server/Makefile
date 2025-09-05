# aesdsocket Makefile

OBJ := aesdsocket

all: $(OBJ)

$(OBJ): $(OBJ).c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ -lpthread

clean:
	rm -f $(OBJ)
