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
('registro 1', '2022-03-15', '2022-03-15 08:30:00', '2022-03-15 08:30:00-05'),
('registro 2', '2022-07-20', '2022-07-20 14:45:00', '2022-07-20 14:45:00-05'),
('registro 3', '2022-11-01', '2022-11-01 23:00:00', '2022-11-01 23:00:00-05');

SELECT * FROM prueba_fechas;

-- diferencia entre TIMESTAMP y TIMESTAMPTZ al cambiar la zona horaria de sesion
SET timezone = 'America/Lima';
SELECT con_hora, con_zona FROM prueba_fechas;

SET timezone = 'America/New_York';
SELECT con_hora, con_zona FROM prueba_fechas;
-- con_hora no cambia, con_zona se ajusta automaticamente
