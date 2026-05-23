#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

#define CONNINFO "host=localhost dbname=netmonitor user=roman password=RodreehPy26"

// ─────────────────────────────────────
//  Conectar a la base de datos
// ─────────────────────────────────────
PGconn *conectar() {
    PGconn *conn = PQconnectdb(CONNINFO);

    if (PQstatus(conn) != CONNECTION_OK) {
        printf("Error al conectar: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }
    return conn;
}

// ─────────────────────────────────────
//  Listar todos los dispositivos
// ─────────────────────────────────────
void listar_dispositivos(PGconn *conn) {
    PGresult *res = PQexec(conn,
        "SELECT id, nombre, ip, mac, tipo, activo FROM dispositivos ORDER BY id"
    );

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("Error en consulta: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int filas = PQntuples(res);
    int cols  = PQnfields(res);

    printf("\n%-5s %-20s %-15s %-20s %-10s %-6s\n",
           "ID", "NOMBRE", "IP", "MAC", "TIPO", "ACTIVO");
    printf("─────────────────────────────────────────────────────────────────\n");

    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%-20s ", PQgetvalue(res, i, j));
        }
        printf("\n");
    }

    printf("\nTotal: %d dispositivo(s)\n\n", filas);
    PQclear(res);
}

// ─────────────────────────────────────
//  Insertar un dispositivo nuevo
// ─────────────────────────────────────
void insertar_dispositivo(PGconn *conn,
                          const char *nombre,
                          const char *ip,
                          const char *mac,
                          const char *tipo) {

    const char *params[4] = {nombre, ip, mac, tipo};

    PGresult *res = PQexecParams(conn,
        "INSERT INTO dispositivos (nombre, ip, mac, tipo) "
        "VALUES ($1, $2, $3, $4)",
        4,        // número de parámetros
        NULL,     // tipos (NULL = PostgreSQL los infiere)
        params,   // valores
        NULL,     // longitudes (NULL = strings terminados en \0)
        NULL,     // formatos (NULL = texto)
        0         // resultado en texto
    );

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("Error al insertar: %s\n", PQerrorMessage(conn));
    } else {
        printf("Dispositivo '%s' agregado correctamente\n", nombre);
    }

    PQclear(res);
}

// ─────────────────────────────────────
//  Main
// ───────────────────────────────────── 
int main() {
    PGconn *conn = conectar();

    printf("=== NetMonitor — Dispositivos ===\n");

    // Insertar un dispositivo nuevo
    insertar_dispositivo(conn, "Router-Casa", "192.168.1.254", "CC:DD:EE:FF:00:11", "router");

    // Listar todos
    listar_dispositivos(conn);

    PQfinish(conn);
    return 0;
}
