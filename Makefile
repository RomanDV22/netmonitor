CC     = gcc
CFLAGS = -I/usr/include/postgresql -Wall
LIBS   = -lpq -lpcap

all: netmonitor captura

netmonitor: netmonitor.c dispositivos.c db.c
	$(CC) netmonitor.c dispositivos.c db.c -o netmonitor $(CFLAGS) $(LIBS)

captura: captura.c db.c
	$(CC) captura.c db.c -o captura $(CFLAGS) $(LIBS)

clean:
	rm -f netmonitor captura dispositivos conexion
