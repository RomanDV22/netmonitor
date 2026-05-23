#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include "dispositivos.h"

// ─────────────────────────────────────
//  Listar dispositivos
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

    printf("\n%-5s %-20s %-16s %-19s %-10s %-6s\n",
           "ID", "NOMBRE", "IP", "MAC", "TIPO", "ACTIVO");
    printf("────────────────────────────────────────────────────────────────────\n");

    for (int i = 0; i < filas; i++) {
        printf("%-5s %-20s %-16s %-19s %-10s %-6s\n",
            PQgetvalue(res, i, 0),
            PQgetvalue(res, i, 1),
            PQgetvalue(res, i, 2),
            PQgetvalue(res, i, 3),
            PQgetvalue(res, i, 4),
            PQgetvalue(res, i, 5));
    }

    printf("────────────────────────────────────────────────────────────────────\n");
    printf("Total: %d dispositivo(s)\n\n", filas);
    PQclear(res);
}

// ─────────────────────────────────────
//  Insertar dispositivo
// ─────────────────────────────────────
void insertar_dispositivo(PGconn *conn, const char *nombre,
                          const char *ip, const char *mac,
                          const char *tipo) {

    const char *params[4] = {nombre, ip, mac, tipo};

    PGresult *res = PQexecParams(conn,
        "INSERT INTO dispositivos (nombre, ip, mac, tipo) VALUES ($1, $2, $3, $4)",
        4, NULL, params, NULL, NULL, 0
    );

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("Error al insertar: %s\n", PQerrorMessage(conn));
    } else {
        printf("✓ Dispositivo '%s' agregado correctamente.\n", nombre);
    }

    PQclear(res);
}

// ─────────────────────────────────────
//  Eliminar dispositivo
// ─────────────────────────────────────
void eliminar_dispositivo(PGconn *conn, int id) {
    char id_str[10];
    snprintf(id_str, sizeof(id_str), "%d", id);

    const char *params[1] = {id_str};

    PGresult *res = PQexecParams(conn,
        "DELETE FROM dispositivos WHERE id = $1",
        1, NULL, params, NULL, NULL, 0
    );

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("Error al eliminar: %s\n", PQerrorMessage(conn));
    } else {
        printf("✓ Dispositivo con ID %d eliminado.\n", id);
    }

    PQclear(res);
}
