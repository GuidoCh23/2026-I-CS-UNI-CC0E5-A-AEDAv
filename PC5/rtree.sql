-- Indice R-Tree en PostgreSQL (via GiST + PostGIS)
-- Soporta consultas espaciales: contencion, interseccion, proximidad

CREATE EXTENSION IF NOT EXISTS postgis;

DROP TABLE IF EXISTS lugares;

CREATE TABLE lugares (
    id     SERIAL PRIMARY KEY,
    nombre VARCHAR(100),
    geom   GEOMETRY(Point, 4326)
);

-- Puntos en Lima (longitud, latitud)
INSERT INTO lugares (nombre, geom) VALUES
('Plaza Mayor',      ST_SetSRID(ST_MakePoint(-77.0282, -12.0464), 4326)),
('Miraflores',       ST_SetSRID(ST_MakePoint(-77.0282, -12.1219), 4326)),
('San Isidro',       ST_SetSRID(ST_MakePoint(-77.0500, -12.0964), 4326)),
('Barranco',         ST_SetSRID(ST_MakePoint(-77.0197, -12.1464), 4326)),
('Surco',            ST_SetSRID(ST_MakePoint(-77.0000, -12.1500), 4326)),
('La Molina',        ST_SetSRID(ST_MakePoint(-76.9333, -12.0833), 4326)),
('Ate',              ST_SetSRID(ST_MakePoint(-76.9500, -12.0167), 4326)),
('San Borja',        ST_SetSRID(ST_MakePoint(-77.0000, -12.1000), 4326)),
('Lince',            ST_SetSRID(ST_MakePoint(-77.0333, -12.0833), 4326)),
('Jesus Maria',      ST_SetSRID(ST_MakePoint(-77.0500, -12.0667), 4326));

-- Sin indice: Seq Scan
EXPLAIN ANALYZE
SELECT * FROM lugares
WHERE ST_DWithin(
    geom::geography,
    ST_SetSRID(ST_MakePoint(-77.0282, -12.0464), 4326)::geography,
    5000
);

CREATE INDEX idx_gist_lugares ON lugares USING GIST (geom);

-- Lugares dentro de 5km de Plaza Mayor
EXPLAIN ANALYZE
SELECT * FROM lugares
WHERE ST_DWithin(
    geom::geography,
    ST_SetSRID(ST_MakePoint(-77.0282, -12.0464), 4326)::geography,
    5000
);

-- Lugares dentro de un rectangulo
EXPLAIN ANALYZE
SELECT * FROM lugares
WHERE ST_Within(
    geom,
    ST_SetSRID(ST_MakeEnvelope(-77.10, -12.15, -76.95, -12.00), 4326)
);

-- 3 lugares mas cercanos a Plaza Mayor
EXPLAIN ANALYZE
SELECT nombre,
    ST_Distance(
        geom::geography,
        ST_SetSRID(ST_MakePoint(-77.0282, -12.0464), 4326)::geography
    ) AS distancia_m
FROM lugares
ORDER BY geom <-> ST_SetSRID(ST_MakePoint(-77.0282, -12.0464), 4326)
LIMIT 3;
