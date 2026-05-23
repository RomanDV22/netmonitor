#ifndef DB_H
#define DB_H

#include <libpq-fe.h>

PGconn *conectar();
void    desconectar(PGconn *conn);

#endif
