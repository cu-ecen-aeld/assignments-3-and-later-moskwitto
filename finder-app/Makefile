CROSS_COMPILE=aarch64-none-linux-gnu-
CC = $(CROSS_COMPILE)gcc
writer: writer.o
	$(CC) -o writer writer.o

writer.o: writer.c
	$(CC) -c writer.c

clean:
	rm -f writer writer.o
