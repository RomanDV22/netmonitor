CC     = gcc
CFLAGS = -I/usr/include/postgresql -Wall
LIBS   = -lpq -lpcap

all: netmonitor captura scanner

netmonitor: netmonitor.c dispositivos.c db.c
	$(CC) netmonitor.c dispositivos.c db.c -o netmonitor $(CFLAGS) $(LIBS)

captura: captura.c db.c
	$(CC) captura.c db.c -o captura $(CFLAGS) $(LIBS)

scanner: scanner.c db.c
	$(CC) scanner.c db.c -o scanner $(CFLAGS) $(LIBS)

clean:
	rm -f netmonitor captura scanner dispositivos conexion
