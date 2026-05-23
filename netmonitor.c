#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"
#include "dispositivos.h"

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
    printf("║      N E T M O N I T O R  ║\n");
    printf("╠════════════════════════════╣\n");
    printf("║  1. Listar dispositivos    ║\n");
    printf("║  2. Agregar dispositivo    ║\n");
    printf("║  3. Eliminar dispositivo   ║\n");
    printf("║  4. Salir                  ║\n");
    printf("╚════════════════════════════╝\n");
    printf("Opción: ");
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

                // Quitar salto de línea de fgets
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
                printf("=== Eliminar dispositivo ===\n");
                listar_dispositivos(conn);
                printf("ID a eliminar: ");
                scanf("%d", &id);
                getchar();
                eliminar_dispositivo(conn, id);
                printf("Presioná Enter para continuar...");
                getchar();
                break;

            case 4:
                limpiar();
                printf("Saliendo de NetMonitor...\n");
                break;

            default:
                printf("Opción inválida.\n");
                break;
        }

    } while (opcion != 4);

    desconectar(conn);
    return 0;
}
