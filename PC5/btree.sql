-- Indice BTree en PostgreSQL
-- Soporta: =, <, <=, >, >=, BETWEEN, ORDER BY

DROP TABLE IF EXISTS productos;

CREATE TABLE productos (
    id      SERIAL PRIMARY KEY,
    nombre  VARCHAR(100),
    precio  NUMERIC(10, 2)
);

INSERT INTO productos (nombre, precio) VALUES
('Laptop',       2500.00),
('Mouse',          35.00),
('Teclado',        80.00),
('Monitor',       750.00),
('Auriculares',   120.00),
('Webcam',         95.00),
('Disco SSD',     300.00),
('Memoria RAM',   180.00),
('Impresora',     450.00),
('Tablet',        600.00);

-- Sin indice: Seq Scan
EXPLAIN ANALYZE
SELECT * FROM productos WHERE precio BETWEEN 100 AND 500;

CREATE INDEX idx_precio ON productos USING BTREE (precio);

-- Con indice: Index Scan
EXPLAIN ANALYZE
SELECT * FROM productos WHERE precio BETWEEN 100 AND 500;

EXPLAIN ANALYZE
SELECT * FROM productos WHERE precio > 400;

-- ORDER BY aprovecha el indice, no necesita ordenar
EXPLAIN ANALYZE
SELECT * FROM productos ORDER BY precio;
