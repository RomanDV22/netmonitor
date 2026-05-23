CC     = gcc
CFLAGS = -I/usr/include/postgresql
LIBS   = -lpq

all: conexion dispositivos

conexion: conexion.c
	$(CC) conexion.c -o conexion $(CFLAGS) $(LIBS)

dispositivos: dispositivos.c
	$(CC) dispositivos.c -o dispositivos $(CFLAGS) $(LIBS)

clean:
	rm -f conexion dispositivos
