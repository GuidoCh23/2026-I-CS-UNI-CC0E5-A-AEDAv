-- Manejo de fechas en PostgreSQL

DROP TABLE IF EXISTS prueba_fechas;

-- DATE: solo fecha, sin hora. Formato: YYYY-MM-DD
-- TIMESTAMP: fecha y hora, sin zona horaria. Formato: YYYY-MM-DD HH:MM:SS
-- TIMESTAMPTZ: fecha y hora con zona horaria. Formato: YYYY-MM-DD HH:MM:SS+ZZ

CREATE TABLE prueba_fechas (
    id           SERIAL PRIMARY KEY,
    descripcion  VARCHAR(50),
    solo_fecha   DATE,
    con_hora     TIMESTAMP,
    con_zona     TIMESTAMPTZ
);

INSERT INTO prueba_fechas (descripcion, solo_fecha, con_hora, con_zona) VALUES
('registro 1',  '2022-01-10', '2022-01-10 07:00:00', '2022-01-10 07:00:00-05'),
('registro 2',  '2022-02-14', '2022-02-14 09:15:00', '2022-02-14 09:15:00-05'),
('registro 3',  '2022-03-15', '2022-03-15 08:30:00', '2022-03-15 08:30:00-05'),
('registro 4',  '2022-04-22', '2022-04-22 11:00:00', '2022-04-22 11:00:00-05'),
('registro 5',  '2022-05-05', '2022-05-05 13:45:00', '2022-05-05 13:45:00-05'),
('registro 6',  '2022-06-18', '2022-06-18 10:20:00', '2022-06-18 10:20:00-05'),
('registro 7',  '2022-07-20', '2022-07-20 14:45:00', '2022-07-20 14:45:00-05'),
('registro 8',  '2022-08-09', '2022-08-09 16:00:00', '2022-08-09 16:00:00-05'),
('registro 9',  '2022-09-30', '2022-09-30 08:10:00', '2022-09-30 08:10:00-05'),
('registro 10', '2022-10-12', '2022-10-12 17:30:00', '2022-10-12 17:30:00-05'),
('registro 11', '2022-11-01', '2022-11-01 23:00:00', '2022-11-01 23:00:00-05'),
('registro 12', '2022-12-25', '2022-12-25 12:00:00', '2022-12-25 12:00:00-05');

SELECT * FROM prueba_fechas;

-- diferencia entre TIMESTAMP y TIMESTAMPTZ al cambiar la zona horaria de sesion
SET timezone = 'America/Lima';
SELECT con_hora, con_zona FROM prueba_fechas;

SET timezone = 'America/New_York';
SELECT con_hora, con_zona FROM prueba_fechas;
-- con_hora no cambia, con_zona se ajusta automaticamente

-- indice sobre columna de fecha para acelerar consultas por rango
CREATE INDEX idx_fecha ON prueba_fechas (solo_fecha);

EXPLAIN ANALYZE
SELECT * FROM prueba_fechas
WHERE solo_fecha BETWEEN '2022-03-01' AND '2022-08-01';
