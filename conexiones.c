#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "conexiones.h"

// ─────────────────────────────────────
//  Listar conexiones (mapa de red)
//  Muestra dispositivo+interfaz en cada
//  extremo, no solo IDs sueltos.
// ─────────────────────────────────────
void listar_conexiones(PGconn *conn) {
    PGresult *res = PQexec(conn,
        "SELECT c.id, "
        "       da.nombre, ia.nombre, "
        "       db2.nombre, ib.nombre, "
        "       c.tipo_cable, c.activa "
        "FROM conexiones c "
        "JOIN interfaces ia  ON ia.id = c.interfaz_a_id "
        "JOIN dispositivos da ON da.id = ia.dispositivo_id "
        "JOIN interfaces ib  ON ib.id = c.interfaz_b_id "
        "JOIN dispositivos db2 ON db2.id = ib.dispositivo_id "
        "ORDER BY c.id"
    );

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("Error en consulta: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int filas = PQntuples(res);

    printf("\n%-4s %-22s %-22s %-12s %-6s\n",
           "ID", "EXTREMO A", "EXTREMO B", "CABLE", "ACTIVA");
    printf("──────────────────────────────────────────────────────────────────────\n");

    for (int i = 0; i < filas; i++) {
        char extremo_a[64], extremo_b[64];
        snprintf(extremo_a, sizeof(extremo_a), "%s (%s)",
                 PQgetvalue(res, i, 1), PQgetvalue(res, i, 2));
        snprintf(extremo_b, sizeof(extremo_b), "%s (%s)",
                 PQgetvalue(res, i, 3), PQgetvalue(res, i, 4));

        printf("%-4s %-22s %-22s %-12s %-6s\n",
            PQgetvalue(res, i, 0),
            extremo_a,
            extremo_b,
            PQgetvalue(res, i, 5),
            PQgetvalue(res, i, 6));
    }

    printf("──────────────────────────────────────────────────────────────────────\n");
    printf("Total: %d conexion(es)\n\n", filas);
    PQclear(res);
}

// ─────────────────────────────────────
//  Insertar conexión entre dos interfaces
// ─────────────────────────────────────
void insertar_conexion(PGconn *conn, int interfaz_a_id, int interfaz_b_id,
                       const char *tipo_cable) {

    if (interfaz_a_id == interfaz_b_id) {
        printf("Error: una interfaz no puede conectarse consigo misma.\n");
        return;
    }

    char a_str[10], b_str[10];
    snprintf(a_str, sizeof(a_str), "%d", interfaz_a_id);
    snprintf(b_str, sizeof(b_str), "%d", interfaz_b_id);

    PGresult *res;

    if (tipo_cable != NULL && strlen(tipo_cable) > 0) {
        const char *params[3] = {a_str, b_str, tipo_cable};
        res = PQexecParams(conn,
            "INSERT INTO conexiones (interfaz_a_id, interfaz_b_id, tipo_cable) "
            "VALUES ($1, $2, $3)",
            3, NULL, params, NULL, NULL, 0);
    } else {
        const char *params[2] = {a_str, b_str};
        res = PQexecParams(conn,
            "INSERT INTO conexiones (interfaz_a_id, interfaz_b_id) "
            "VALUES ($1, $2)",
            2, NULL, params, NULL, NULL, 0);
    }

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("Error al insertar: %s\n", PQerrorMessage(conn));
    } else {
        printf("✓ Conexión creada correctamente.\n");
    }

    PQclear(res);
}

// ─────────────────────────────────────
//  Eliminar conexión
// ─────────────────────────────────────
void eliminar_conexion(PGconn *conn, int id) {
    char id_str[10];
    snprintf(id_str, sizeof(id_str), "%d", id);

    const char *params[1] = {id_str};

    PGresult *res = PQexecParams(conn,
        "DELETE FROM conexiones WHERE id = $1",
        1, NULL, params, NULL, NULL, 0
    );

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("Error al eliminar: %s\n", PQerrorMessage(conn));
    } else {
        printf("✓ Conexión con ID %d eliminada.\n", id);
    }

    PQclear(res);
}
