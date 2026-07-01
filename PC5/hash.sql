-- Indice Hash en PostgreSQL
-- Solo soporta igualdad exacta (=)
-- No sirve para rangos ni ORDER BY

DROP TABLE IF EXISTS usuarios;

CREATE TABLE usuarios (
    id     SERIAL PRIMARY KEY,
    email  VARCHAR(150),
    nombre VARCHAR(80),
    pais   CHAR(2)
);

INSERT INTO usuarios (email, nombre, pais) VALUES
('ana@mail.com',     'Ana Torres',    'PE'),
('luis@mail.com',    'Luis Gomez',    'MX'),
('carla@mail.com',   'Carla Rios',    'AR'),
('pedro@mail.com',   'Pedro Salas',   'PE'),
('maria@mail.com',   'Maria Leon',    'CO'),
('jose@mail.com',    'Jose Vargas',   'CL'),
('lucia@mail.com',   'Lucia Paredes', 'PE'),
('diego@mail.com',   'Diego Mora',    'MX'),
('sofia@mail.com',   'Sofia Castro',  'AR'),
('andres@mail.com',  'Andres Ruiz',   'PE');

-- Sin indice: Seq Scan
EXPLAIN ANALYZE
SELECT * FROM usuarios WHERE email = 'carla@mail.com';

CREATE INDEX idx_hash_email ON usuarios USING HASH (email);

-- Con Hash: busqueda exacta
EXPLAIN ANALYZE
SELECT * FROM usuarios WHERE email = 'carla@mail.com';

-- Hash no se usa en rangos
EXPLAIN ANALYZE
SELECT * FROM usuarios WHERE id > 5;

-- Hash no se usa en ORDER BY
EXPLAIN ANALYZE
SELECT * FROM usuarios ORDER BY email;
