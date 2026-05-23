#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

int main() {

    // Cadena de conexión
    const char *conninfo = "host=localhost "
                           "dbname=netmonitor "
                           "user=roman "
                           "password=RodreehPy26";

    // Intentar conectar
    PGconn *conn = PQconnectdb(conninfo);

    // Verificar la conexión
    if (PQstatus(conn) != CONNECTION_OK) {
        printf("Error al conectar: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    printf("Conexión exitosa a NetMonitor\n");
    printf("Base de datos: %s\n", PQdb(conn));
    printf("Usuario: %s\n", PQuser(conn));
    printf("Host: %s\n", PQhost(conn));

    // Cerrar conexión
    PQfinish(conn);
    return 0;
}
