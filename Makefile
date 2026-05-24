CC     = gcc
CFLAGS = -I/usr/include/postgresql -Wall
LIBS   = -lpq -lpcap

all: netmonitor captura scanner servidor

netmonitor: netmonitor.c dispositivos.c db.c
	$(CC) netmonitor.c dispositivos.c db.c -o netmonitor $(CFLAGS) $(LIBS)

captura: captura.c db.c
	$(CC) captura.c db.c -o captura $(CFLAGS) $(LIBS)

scanner: scanner.c db.c
	$(CC) scanner.c db.c -o scanner $(CFLAGS) $(LIBS)

servidor: servidor.c db.c
	$(CC) servidor.c db.c -o servidor $(CFLAGS) $(LIBS)

clean:
	rm -f netmonitor captura scanner servidor dispositivos conexion
