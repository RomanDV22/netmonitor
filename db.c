#include <stdio.h>
#include <stdlib.h>
#include "db.h"

#define CONNINFO "host=localhost dbname=netmonitor_dev user=claude password=12345"

PGconn *conectar() {
    PGconn *conn = PQconnectdb(CONNINFO);

    if (PQstatus(conn) != CONNECTION_OK) {
        printf("Error al conectar: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }
    return conn;
}

void desconectar(PGconn *conn) {
    PQfinish(conn);
    printf("Conexión cerrada.\n");
}
