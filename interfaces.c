#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "interfaces.h"

// ─────────────────────────────────────
//  Listar interfaces de un dispositivo
// ─────────────────────────────────────
void listar_interfaces(PGconn *conn, int dispositivo_id) {
    char id_str[10];
    snprintf(id_str, sizeof(id_str), "%d", dispositivo_id);
    const char *params[1] = {id_str};

    PGresult *res = PQexecParams(conn,
        "SELECT id, nombre, ip, mac, tipo, activa FROM interfaces "
        "WHERE dispositivo_id = $1 ORDER BY id",
        1, NULL, params, NULL, NULL, 0
    );

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("Error en consulta: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int filas = PQntuples(res);

    printf("\n%-5s %-15s %-16s %-19s %-10s %-6s\n",
           "ID", "NOMBRE", "IP", "MAC", "TIPO", "ACTIVA");
    printf("────────────────────────────────────────────────────────────────────\n");

    for (int i = 0; i < filas; i++) {
        printf("%-5s %-15s %-16s %-19s %-10s %-6s\n",
            PQgetvalue(res, i, 0),
            PQgetvalue(res, i, 1),
            PQgetvalue(res, i, 2),
            PQgetvalue(res, i, 3),
            PQgetvalue(res, i, 4),
            PQgetvalue(res, i, 5));
    }

    printf("────────────────────────────────────────────────────────────────────\n");
    printf("Total: %d interfaz(es)\n\n", filas);
    PQclear(res);
}

// ─────────────────────────────────────
//  Insertar interfaz
// ─────────────────────────────────────
void insertar_interfaz(PGconn *conn, int dispositivo_id, const char *nombre,
                       const char *ip, const char *mac, const char *tipo) {

    char disp_str[10];
    snprintf(disp_str, sizeof(disp_str), "%d", dispositivo_id);
    const char *params[5] = {disp_str, nombre, ip, mac, tipo};

    PGresult *res = PQexecParams(conn,
        "INSERT INTO interfaces (dispositivo_id, nombre, ip, mac, tipo) "
        "VALUES ($1, $2, $3, $4, $5)",
        5, NULL, params, NULL, NULL, 0
    );

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("Error al insertar: %s\n", PQerrorMessage(conn));
    } else {
        printf("✓ Interfaz '%s' agregada correctamente.\n", nombre);
    }

    PQclear(res);
}

// ─────────────────────────────────────
//  Editar interfaz (IP y MAC)
// ─────────────────────────────────────
void editar_interfaz(PGconn *conn, int id, const char *nueva_ip, const char *nueva_mac) {
    char id_str[10];
    snprintf(id_str, sizeof(id_str), "%d", id);

    if (strlen(nueva_ip) > 0 && strlen(nueva_mac) > 0) {
        const char *params[3] = {nueva_ip, nueva_mac, id_str};
        PGresult *res = PQexecParams(conn,
            "UPDATE interfaces SET ip = $1, mac = $2 WHERE id = $3",
            3, NULL, params, NULL, NULL, 0);
        if (PQresultStatus(res) == PGRES_COMMAND_OK)
            printf("✓ IP y MAC actualizadas.\n");
        else
            printf("Error: %s\n", PQerrorMessage(conn));
        PQclear(res);

    } else if (strlen(nueva_ip) > 0) {
        const char *params[2] = {nueva_ip, id_str};
        PGresult *res = PQexecParams(conn,
            "UPDATE interfaces SET ip = $1 WHERE id = $2",
            2, NULL, params, NULL, NULL, 0);
        if (PQresultStatus(res) == PGRES_COMMAND_OK)
            printf("✓ IP actualizada.\n");
        else
            printf("Error: %s\n", PQerrorMessage(conn));
        PQclear(res);

    } else if (strlen(nueva_mac) > 0) {
        const char *params[2] = {nueva_mac, id_str};
        PGresult *res = PQexecParams(conn,
            "UPDATE interfaces SET mac = $1 WHERE id = $2",
            2, NULL, params, NULL, NULL, 0);
        if (PQresultStatus(res) == PGRES_COMMAND_OK)
            printf("✓ MAC actualizada.\n");
        else
            printf("Error: %s\n", PQerrorMessage(conn));
        PQclear(res);

    } else {
        printf("No se modificó ningún campo.\n");
    }
}

// ─────────────────────────────────────
//  Eliminar interfaz
// ─────────────────────────────────────
void eliminar_interfaz(PGconn *conn, int id) {
    char id_str[10];
    snprintf(id_str, sizeof(id_str), "%d", id);

    const char *params[1] = {id_str};

    PGresult *res = PQexecParams(conn,
        "DELETE FROM interfaces WHERE id = $1",
        1, NULL, params, NULL, NULL, 0
    );

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("Error al eliminar: %s\n", PQerrorMessage(conn));
    } else {
        printf("✓ Interfaz con ID %d eliminada.\n", id);
    }

    PQclear(res);
}
