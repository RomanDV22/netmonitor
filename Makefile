CC     = gcc
CFLAGS = -I/usr/include/postgresql
LIBS   = -lpq

all: conexion

conexion: conexion.c
	$(CC) conexion.c -o conexion $(CFLAGS) $(LIBS)

clean:
	rm -f conexion
