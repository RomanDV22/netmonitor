-- ─────────────────────────────────────
--  NetMonitor — Esquema de base de datos
--  Etapa 1 — Diseño inicial
-- ─────────────────────────────────────

CREATE TABLE dispositivos (
    id          SERIAL PRIMARY KEY,
    nombre      VARCHAR(100) NOT NULL,
    ip          VARCHAR(15),
    mac         VARCHAR(17),
    tipo        VARCHAR(50),
    activo      BOOLEAN DEFAULT TRUE,
    created_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE interfaces (
    id                SERIAL PRIMARY KEY,
    dispositivo_id    INTEGER REFERENCES dispositivos(id),
    nombre            VARCHAR(50) NOT NULL,
    ip                VARCHAR(15),
    mac               VARCHAR(17),
    tipo              VARCHAR(20),
    activa            BOOLEAN DEFAULT TRUE
);

CREATE TABLE trafico (
    id              SERIAL PRIMARY KEY,
    interfaz_id     INTEGER REFERENCES interfaces(id),
    timestamp       TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    protocolo       VARCHAR(10),
    ip_origen       VARCHAR(15),
    ip_destino      VARCHAR(15),
    puerto_origen   INTEGER,
    puerto_destino  INTEGER,
    tamanio         INTEGER,
    info            TEXT
);

-- ─────────────────────────────────────
--  Etapa 2 — Topología de red
-- ─────────────────────────────────────

-- Une dos interfaces (de dispositivos distintos, normalmente).
-- Modela el "cable" entre un puerto y otro, como en Packet Tracer.
CREATE TABLE conexiones (
    id              SERIAL PRIMARY KEY,
    interfaz_a_id   INTEGER REFERENCES interfaces(id),
    interfaz_b_id   INTEGER REFERENCES interfaces(id),
    tipo_cable      VARCHAR(30) DEFAULT 'ethernet',
    activa          BOOLEAN DEFAULT TRUE,
    created_at      TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CHECK (interfaz_a_id <> interfaz_b_id)
);
