CC     = gcc
CFLAGS = -I/usr/include/postgresql -Wall
LIBS   = -lpq

all: netmonitor

netmonitor: netmonitor.c dispositivos.c db.c
	$(CC) netmonitor.c dispositivos.c db.c -o netmonitor $(CFLAGS) $(LIBS)

clean:
	rm -f netmonitor dispositivos conexion
