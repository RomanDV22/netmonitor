#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

#define RED_BASE   "192.168.0."
#define IP_INICIO  1
#define IP_FIN     254

// ─────────────────────────────────────
//  Verificar si una IP responde al ping
// ─────────────────────────────────────
int ping_host(const char *ip) {
    char comando[100];
    // -c 1 = un solo ping
    // -W 1 = timeout de 1 segundo
    // > /dev/null 2>&1 = suprimir salida
    snprintf(comando, sizeof(comando),
             "ping -c 1 -W 1 %s > /dev/null 2>&1", ip);
    return system(comando) == 0;
}

// ─────────────────────────────────────
//  Guardar dispositivo si no existe
// ─────────────────────────────────────
void guardar_si_no_existe(PGconn *conn, const char *ip) {
    const char *params[1] = {ip};

    // Verificar si ya existe
    PGresult *res = PQexecParams(conn,
        "SELECT id FROM dispositivos WHERE ip = $1",
        1, NULL, params, NULL, NULL, 0
    );

    if (PQntuples(res) == 0) {
        // No existe — insertar
        PQclear(res);

        res = PQexecParams(conn,
            "INSERT INTO dispositivos (nombre, ip, tipo) "
	    "VALUES ('desconocido', $1, 'desconocido')",
            1, NULL, params, NULL, NULL, 0
        );
        printf("  [+] Nuevo dispositivo encontrado: %s\n", ip);
    } else {
        printf("  [=] Ya registrado: %s\n", ip);
    }

    PQclear(res);
}

// ─────────────────────────────────────
//  Escanear el rango de red
// ─────────────────────────────────────
void escanear_red(PGconn *conn) {
    char ip[20];
    int activos = 0;

    printf("\n=== Escaneando red %s%d - %s%d ===\n\n",
           RED_BASE, IP_INICIO, RED_BASE, IP_FIN);

    for (int i = IP_INICIO; i <= IP_FIN; i++) {
        snprintf(ip, sizeof(ip), "%s%d", RED_BASE, i);
        printf("Probando %s...\r", ip);
        fflush(stdout);

        if (ping_host(ip)) {
            activos++;
            printf("  ✓ Activo: %-20s\n", ip);
            guardar_si_no_existe(conn, ip);
        }
    }

    printf("\n─────────────────────────────────\n");
    printf("Escaneo completo.\n");
    printf("Dispositivos activos encontrados: %d\n\n", activos);
}

// ─────────────────────────────────────
//  Main
// ─────────────────────────────────────
int main() {
    PGconn *conn = conectar();
    printf("✓ Conectado a la DB\n");

    escanear_red(conn);

    // Mostrar mapa final
    printf("=== Mapa de red actual ===\n\n");
    PGresult *res = PQexec(conn,
        "SELECT id, nombre, ip, tipo, activo "
        "FROM dispositivos ORDER BY ip"
    );

    printf("%-5s %-20s %-16s %-12s %-6s\n",
           "ID", "NOMBRE", "IP", "TIPO", "ACTIVO");
    printf("──────────────────────────────────────────────────────\n");

    for (int i = 0; i < PQntuples(res); i++) {
        printf("%-5s %-20s %-16s %-12s %-6s\n",
            PQgetvalue(res, i, 0),
            PQgetvalue(res, i, 1),
            PQgetvalue(res, i, 2),
            PQgetvalue(res, i, 3),
            PQgetvalue(res, i, 4));
    }

    PQclear(res);
    desconectar(conn);
    return 0;
}
