#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"
#include "dispositivos.h"
#include "interfaces.h"
#include "conexiones.h"

#define MAX 100

// ─────────────────────────────────────
//  Limpiar pantalla
// ─────────────────────────────────────
void limpiar() {
    system("clear");
}

// ─────────────────────────────────────
//  Menú principal
// ─────────────────────────────────────
void menu() {
    printf("╔════════════════════════════╗\n");
    printf("║     N E T M O N I T O R    ║\n");
    printf("╠════════════════════════════╣\n");
    printf("║  1. Listar dispositivos    ║\n");
    printf("║  2. Agregar dispositivo    ║\n");
    printf("║  3. Editar dispositivo     ║\n");
    printf("║  4. Eliminar dispositivo   ║\n");
    printf("║  5. Ver trafico capturado  ║\n");
    printf("║  6. Gestionar interfaces   ║\n");
    printf("║  7. Gestionar conexiones   ║\n");
    printf("║  8. Salir                  ║\n");
    printf("╚════════════════════════════╝\n");
    printf("Opción: ");
}

// ─────────────────────────────────────
//  Ver tráfico capturado
// ─────────────────────────────────────
void ver_trafico(PGconn *conn) {
    printf("=== Trafico capturado ===\n\n");
    printf("Cuantos registros queres ver? (max 50): ");

    int limite;
    scanf("%d", &limite);
    getchar();

    if (limite <= 0 || limite > 50) limite = 10;

    char query[256];
    snprintf(query, sizeof(query),
        "SELECT protocolo, ip_origen, puerto_origen, "
        "ip_destino, puerto_destino, tamanio, timestamp "
        "FROM trafico ORDER BY id DESC LIMIT %d", limite);

    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("Error en consulta: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int filas = PQntuples(res);
    printf("\n%-6s %-16s %-6s %-16s %-6s %-6s %-20s\n",
           "PROTO", "IP ORIGEN", "P.OR", "IP DESTINO", "P.DS", "BYTES", "TIMESTAMP");
    printf("────────────────────────────────────────────────────────────────────────────────\n");

    for (int i = 0; i < filas; i++) {
        printf("%-6s %-16s %-6s %-16s %-6s %-6s %-20s\n",
            PQgetvalue(res, i, 0),
            PQgetvalue(res, i, 1),
            PQgetvalue(res, i, 2),
            PQgetvalue(res, i, 3),
            PQgetvalue(res, i, 4),
            PQgetvalue(res, i, 5),
            PQgetvalue(res, i, 6));
    }

    printf("────────────────────────────────────────────────────────────────────────────────\n");
    printf("Total mostrado: %d registro(s)\n\n", filas);

    // Resumen por protocolo
    printf("── Resumen por protocolo ──\n");
    PGresult *res2 = PQexec(conn,
        "SELECT protocolo, COUNT(*) as total FROM trafico "
        "GROUP BY protocolo ORDER BY total DESC"
    );

    if (PQresultStatus(res2) == PGRES_TUPLES_OK) {
        for (int i = 0; i < PQntuples(res2); i++) {
            printf("  %-6s → %s paquetes\n",
                PQgetvalue(res2, i, 0),
                PQgetvalue(res2, i, 1));
        }
    }

    PQclear(res);
    PQclear(res2);
}

// ─────────────────────────────────────
//  Editar dispositivo (IP y MAC)
// ─────────────────────────────────────
void editar_dispositivo(PGconn *conn) {
    printf("=== Editar dispositivo ===\n\n");

    // Mostrar lista para que el usuario elija
    listar_dispositivos(conn);

    printf("ID a editar: ");
    int id;
    scanf("%d", &id);
    getchar();

    char nueva_ip[MAX], nueva_mac[MAX];

    printf("\nDeja en blanco para no modificar el campo.\n\n");

    printf("Nueva IP  : "); fgets(nueva_ip,  MAX, stdin);
    printf("Nueva MAC : "); fgets(nueva_mac, MAX, stdin);

    nueva_ip [strcspn(nueva_ip,  "\n")] = 0;
    nueva_mac[strcspn(nueva_mac, "\n")] = 0;

    // Actualizar solo los campos que no quedaron vacíos
    if (strlen(nueva_ip) > 0 && strlen(nueva_mac) > 0) {
        char id_str[10];
        snprintf(id_str, sizeof(id_str), "%d", id);
        const char *params[3] = {nueva_ip, nueva_mac, id_str};
        PGresult *res = PQexecParams(conn,
            "UPDATE dispositivos SET ip = $1, mac = $2 WHERE id = $3",
            3, NULL, params, NULL, NULL, 0);
        if (PQresultStatus(res) == PGRES_COMMAND_OK)
            printf("✓ IP y MAC actualizadas.\n");
        else
            printf("Error: %s\n", PQerrorMessage(conn));
        PQclear(res);

    } else if (strlen(nueva_ip) > 0) {
        char id_str[10];
        snprintf(id_str, sizeof(id_str), "%d", id);
        const char *params[2] = {nueva_ip, id_str};
        PGresult *res = PQexecParams(conn,
            "UPDATE dispositivos SET ip = $1 WHERE id = $2",
            2, NULL, params, NULL, NULL, 0);
        if (PQresultStatus(res) == PGRES_COMMAND_OK)
            printf("✓ IP actualizada.\n");
        else
            printf("Error: %s\n", PQerrorMessage(conn));
        PQclear(res);

    } else if (strlen(nueva_mac) > 0) {
        char id_str[10];
        snprintf(id_str, sizeof(id_str), "%d", id);
        const char *params[2] = {nueva_mac, id_str};
        PGresult *res = PQexecParams(conn,
            "UPDATE dispositivos SET mac = $1 WHERE id = $2",
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
//  Gestionar interfaces de un dispositivo
// ─────────────────────────────────────
void gestionar_interfaces(PGconn *conn) {
    printf("=== Gestionar interfaces ===\n\n");

    int disp_id, op, id;
    char nombre[MAX], ip[MAX], mac[MAX], tipo[MAX];
    char nueva_ip[MAX], nueva_mac[MAX];

    printf("ID del dispositivo: ");
    scanf("%d", &disp_id);
    getchar();

    listar_interfaces(conn, disp_id);

    printf("1. Agregar interfaz\n");
    printf("2. Editar interfaz\n");
    printf("3. Eliminar interfaz\n");
    printf("0. Volver\n");
    printf("Opción: ");
    scanf("%d", &op);
    getchar();

    switch (op) {
        case 1:
            printf("Nombre : "); fgets(nombre, MAX, stdin);
            printf("IP     : "); fgets(ip,     MAX, stdin);
            printf("MAC    : "); fgets(mac,    MAX, stdin);
            printf("Tipo   : "); fgets(tipo,   MAX, stdin);

            nombre[strcspn(nombre, "\n")] = 0;
            ip    [strcspn(ip,     "\n")] = 0;
            mac   [strcspn(mac,    "\n")] = 0;
            tipo  [strcspn(tipo,   "\n")] = 0;

            insertar_interfaz(conn, disp_id, nombre, ip, mac, tipo);
            break;

        case 2:
            printf("ID de interfaz a editar: ");
            scanf("%d", &id);
            getchar();

            printf("Deja en blanco para no modificar el campo.\n\n");
            printf("Nueva IP  : "); fgets(nueva_ip,  MAX, stdin);
            printf("Nueva MAC : "); fgets(nueva_mac, MAX, stdin);

            nueva_ip [strcspn(nueva_ip,  "\n")] = 0;
            nueva_mac[strcspn(nueva_mac, "\n")] = 0;

            editar_interfaz(conn, id, nueva_ip, nueva_mac);
            break;

        case 3:
            printf("ID de interfaz a eliminar: ");
            scanf("%d", &id);
            getchar();
            eliminar_interfaz(conn, id);
            break;

        default:
            break;
    }
}

// ─────────────────────────────────────
//  Gestionar conexiones entre interfaces
// ─────────────────────────────────────
void gestionar_conexiones(PGconn *conn) {
    printf("=== Gestionar conexiones ===\n\n");

    int op, id, a, b;
    char tipo_cable[MAX];

    listar_conexiones(conn);

    printf("1. Agregar conexión\n");
    printf("2. Eliminar conexión\n");
    printf("0. Volver\n");
    printf("Opción: ");
    scanf("%d", &op);
    getchar();

    switch (op) {
        case 1:
            printf("ID interfaz A: ");
            scanf("%d", &a);
            getchar();
            printf("ID interfaz B: ");
            scanf("%d", &b);
            getchar();

            printf("Tipo de cable (Enter = ethernet): ");
            fgets(tipo_cable, MAX, stdin);
            tipo_cable[strcspn(tipo_cable, "\n")] = 0;

            insertar_conexion(conn, a, b, tipo_cable);
            break;

        case 2:
            printf("ID de conexión a eliminar: ");
            scanf("%d", &id);
            getchar();
            eliminar_conexion(conn, id);
            break;

        default:
            break;
    }
}

// ─────────────────────────────────────
//  Main
// ─────────────────────────────────────
int main() {
    PGconn *conn = conectar();
    int opcion;
    char nombre[MAX], ip[MAX], mac[MAX], tipo[MAX];
    int id;

    do {
        limpiar();
        menu();
        scanf("%d", &opcion);
        getchar(); // limpiar buffer

        switch (opcion) {
            case 1:
                limpiar();
                printf("=== Dispositivos en la red ===\n");
                listar_dispositivos(conn);
                printf("Presioná Enter para continuar...");
                getchar();
                break;

            case 2:
                limpiar();
                printf("=== Agregar dispositivo ===\n");
                printf("Nombre : "); fgets(nombre, MAX, stdin);
                printf("IP     : "); fgets(ip,     MAX, stdin);
                printf("MAC    : "); fgets(mac,    MAX, stdin);
                printf("Tipo   : "); fgets(tipo,   MAX, stdin);

                nombre[strcspn(nombre, "\n")] = 0;
                ip    [strcspn(ip,     "\n")] = 0;
                mac   [strcspn(mac,    "\n")] = 0;
                tipo  [strcspn(tipo,   "\n")] = 0;

                insertar_dispositivo(conn, nombre, ip, mac, tipo);
                printf("Presioná Enter para continuar...");
                getchar();
                break;

            case 3:
                limpiar();
                editar_dispositivo(conn);
                printf("Presioná Enter para continuar...");
                getchar();
                break;

            case 4:
                limpiar();
                printf("=== Eliminar dispositivo ===\n");
                listar_dispositivos(conn);
                printf("ID a eliminar: ");
                scanf("%d", &id);
                getchar();
                eliminar_dispositivo(conn, id);
                printf("Presioná Enter para continuar...");
                getchar();
                break;

            case 5:
                limpiar();
                ver_trafico(conn);
                printf("Presioná Enter para continuar...");
                getchar();
                break;

            case 6:
                limpiar();
                gestionar_interfaces(conn);
                printf("Presioná Enter para continuar...");
                getchar();
                break;

            case 7:
                limpiar();
                gestionar_conexiones(conn);
                printf("Presioná Enter para continuar...");
                getchar();
                break;

            case 8:
                limpiar();
                printf("Saliendo de NetMonitor...\n");
                break;

            default:
                printf("Opción inválida.\n");
                break;
        }

    } while (opcion != 8);

    desconectar(conn);
    return 0;
}
