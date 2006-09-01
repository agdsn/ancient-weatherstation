--
-- PostgreSQL database dump
--

SET client_encoding = 'SQL_ASCII';
SET check_function_bodies = false;

SET SESSION AUTHORIZATION 'postgres';

--
-- TOC entry 2 (OID 0)
-- Name: wetter; Type: DATABASE; Schema: -; Owner: postgres
--

CREATE DATABASE wetter WITH TEMPLATE = template0 ENCODING = 'SQL_ASCII';


\connect wetter postgres

SET client_encoding = 'SQL_ASCII';
SET check_function_bodies = false;

--
-- TOC entry 4 (OID 2200)
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
GRANT ALL ON SCHEMA public TO PUBLIC;


SET SESSION AUTHORIZATION 'losinshi';

SET search_path = public, pg_catalog;

--
-- TOC entry 6 (OID 18433839)
-- Name: typen; Type: TABLE; Schema: public; Owner: losinshi
--

CREATE TABLE typen (
    typ integer NOT NULL,
    bezeichnung character varying(255),
    tabelle character varying(100)
);


--
-- TOC entry 7 (OID 18433851)
-- Name: sensoren; Type: TABLE; Schema: public; Owner: losinshi
--

CREATE TABLE sensoren (
    id integer DEFAULT nextval('sensoren_id_seq'::text) NOT NULL,
    typ integer,
    addresse integer,
    standort character varying(255),
    beschreibung character varying(255)
);


--
-- TOC entry 8 (OID 18433869)
-- Name: auss1_data; Type: TABLE; Schema: public; Owner: losinshi
--

CREATE TABLE auss1_data (
    "timestamp" timestamp without time zone DEFAULT ('now'::text)::timestamp without time zone NOT NULL,
    sens_id integer NOT NULL,
    "temp" integer
);


--
-- TOC entry 9 (OID 18433877)
-- Name: auss2_data; Type: TABLE; Schema: public; Owner: losinshi
--

CREATE TABLE auss2_data (
    "timestamp" timestamp without time zone DEFAULT ('now'::text)::timestamp without time zone NOT NULL,
    sens_id integer NOT NULL,
    "temp" integer,
    hum integer
);


--
-- TOC entry 5 (OID 18433885)
-- Name: sensoren_id_seq; Type: SEQUENCE; Schema: public; Owner: losinshi
--

CREATE SEQUENCE sensoren_id_seq
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- TOC entry 10 (OID 18433888)
-- Name: regen_data; Type: TABLE; Schema: public; Owner: losinshi
--

CREATE TABLE regen_data (
    "timestamp" timestamp without time zone DEFAULT ('now'::text)::timestamp without time zone NOT NULL,
    sens_id integer NOT NULL,
    count integer
);


--
-- TOC entry 11 (OID 18433896)
-- Name: winds_data; Type: TABLE; Schema: public; Owner: losinshi
--

CREATE TABLE winds_data (
    "timestamp" timestamp without time zone DEFAULT ('now'::text)::timestamp without time zone NOT NULL,
    sens_id integer NOT NULL,
    geschw integer,
    schwank integer,
    richt integer
);


--
-- TOC entry 12 (OID 18433904)
-- Name: innen_data; Type: TABLE; Schema: public; Owner: losinshi
--

CREATE TABLE innen_data (
    "timestamp" timestamp without time zone DEFAULT ('now'::text)::timestamp without time zone NOT NULL,
    sens_id integer NOT NULL,
    "temp" integer,
    hum integer,
    press integer
);


--
-- TOC entry 13 (OID 18433924)
-- Name: pyano_data; Type: TABLE; Schema: public; Owner: losinshi
--

CREATE TABLE pyano_data (
    "timestamp" timestamp without time zone DEFAULT ('now'::text)::timestamp without time zone NOT NULL,
    sens_id integer NOT NULL,
    intens integer
);


--
-- TOC entry 14 (OID 18433932)
-- Name: helli_data; Type: TABLE; Schema: public; Owner: losinshi
--

CREATE TABLE helli_data (
    "timestamp" timestamp without time zone DEFAULT ('now'::text)::timestamp without time zone NOT NULL,
    sens_id integer NOT NULL,
    bight integer
);


--
-- TOC entry 28 (OID 18433947)
-- Name: get_sensor_id(integer, integer); Type: FUNCTION; Schema: public; Owner: losinshi
--

CREATE FUNCTION get_sensor_id(integer, integer) RETURNS integer
    AS 'SELECT id FROM sensoren WHERE sensoren.typ=$1 and addresse=$2'
    LANGUAGE sql;


--
-- TOC entry 15 (OID 19084658)
-- Name: regen_data_year; Type: VIEW; Schema: public; Owner: losinshi
--

CREATE VIEW regen_data_year AS
    SELECT regen_data.sens_id, date_trunc('year'::text, regen_data."timestamp") AS ts, sum(regen_data.count) AS val FROM regen_data GROUP BY date_trunc('year'::text, regen_data."timestamp"), regen_data.sens_id;


--
-- TOC entry 16 (OID 19084661)
-- Name: regen_data_month; Type: VIEW; Schema: public; Owner: losinshi
--

CREATE VIEW regen_data_month AS
    SELECT regen_data.sens_id, date_trunc('month'::text, regen_data."timestamp") AS ts, sum(regen_data.count) AS val FROM regen_data GROUP BY date_trunc('month'::text, regen_data."timestamp"), regen_data.sens_id;


--
-- TOC entry 17 (OID 19084666)
-- Name: regen_data_day; Type: VIEW; Schema: public; Owner: losinshi
--

CREATE VIEW regen_data_day AS
    SELECT regen_data.sens_id, date_trunc('day'::text, regen_data."timestamp") AS ts, sum(regen_data.count) AS val FROM regen_data GROUP BY date_trunc('day'::text, regen_data."timestamp"), regen_data.sens_id;


--
-- TOC entry 18 (OID 19084670)
-- Name: regen_data_hour; Type: VIEW; Schema: public; Owner: losinshi
--

CREATE VIEW regen_data_hour AS
    SELECT regen_data.sens_id, date_trunc('hour'::text, regen_data."timestamp") AS ts, sum(regen_data.count) AS val FROM regen_data GROUP BY date_trunc('hour'::text, regen_data."timestamp"), regen_data.sens_id;


--
-- TOC entry 19 (OID 18433849)
-- Name: typen_pkey; Type: CONSTRAINT; Schema: public; Owner: losinshi
--

ALTER TABLE ONLY typen
    ADD CONSTRAINT typen_pkey PRIMARY KEY (typ);


--
-- TOC entry 20 (OID 18433853)
-- Name: sensoren_pkey; Type: CONSTRAINT; Schema: public; Owner: losinshi
--

ALTER TABLE ONLY sensoren
    ADD CONSTRAINT sensoren_pkey PRIMARY KEY (id);


--
-- TOC entry 21 (OID 18433871)
-- Name: auss1_data_pkey; Type: CONSTRAINT; Schema: public; Owner: losinshi
--

ALTER TABLE ONLY auss1_data
    ADD CONSTRAINT auss1_data_pkey PRIMARY KEY ("timestamp");


--
-- TOC entry 22 (OID 18433879)
-- Name: auss2_data_pkey; Type: CONSTRAINT; Schema: public; Owner: losinshi
--

ALTER TABLE ONLY auss2_data
    ADD CONSTRAINT auss2_data_pkey PRIMARY KEY ("timestamp");


--
-- TOC entry 23 (OID 18433890)
-- Name: regen_data_pkey; Type: CONSTRAINT; Schema: public; Owner: losinshi
--

ALTER TABLE ONLY regen_data
    ADD CONSTRAINT regen_data_pkey PRIMARY KEY ("timestamp");


--
-- TOC entry 24 (OID 18433898)
-- Name: winds_data_pkey; Type: CONSTRAINT; Schema: public; Owner: losinshi
--

ALTER TABLE ONLY winds_data
    ADD CONSTRAINT winds_data_pkey PRIMARY KEY ("timestamp");


--
-- TOC entry 25 (OID 18433906)
-- Name: innen_data_pkey; Type: CONSTRAINT; Schema: public; Owner: losinshi
--

ALTER TABLE ONLY innen_data
    ADD CONSTRAINT innen_data_pkey PRIMARY KEY ("timestamp");


--
-- TOC entry 26 (OID 18433926)
-- Name: pyano_data_pkey; Type: CONSTRAINT; Schema: public; Owner: losinshi
--

ALTER TABLE ONLY pyano_data
    ADD CONSTRAINT pyano_data_pkey PRIMARY KEY ("timestamp");


--
-- TOC entry 27 (OID 18433934)
-- Name: helli_data_pkey; Type: CONSTRAINT; Schema: public; Owner: losinshi
--

ALTER TABLE ONLY helli_data
    ADD CONSTRAINT helli_data_pkey PRIMARY KEY ("timestamp");


--
-- TOC entry 30 (OID 18433855)
-- Name: $1; Type: FK CONSTRAINT; Schema: public; Owner: losinshi
--

ALTER TABLE ONLY sensoren
    ADD CONSTRAINT "$1" FOREIGN KEY (typ) REFERENCES typen(typ);


--
-- TOC entry 31 (OID 18433873)
-- Name: $1; Type: FK CONSTRAINT; Schema: public; Owner: losinshi
--

ALTER TABLE ONLY auss1_data
    ADD CONSTRAINT "$1" FOREIGN KEY (sens_id) REFERENCES sensoren(id);


--
-- TOC entry 32 (OID 18433881)
-- Name: $1; Type: FK CONSTRAINT; Schema: public; Owner: losinshi
--

ALTER TABLE ONLY auss2_data
    ADD CONSTRAINT "$1" FOREIGN KEY (sens_id) REFERENCES sensoren(id);


--
-- TOC entry 33 (OID 18433892)
-- Name: $1; Type: FK CONSTRAINT; Schema: public; Owner: losinshi
--

ALTER TABLE ONLY regen_data
    ADD CONSTRAINT "$1" FOREIGN KEY (sens_id) REFERENCES sensoren(id);


--
-- TOC entry 34 (OID 18433900)
-- Name: $1; Type: FK CONSTRAINT; Schema: public; Owner: losinshi
--

ALTER TABLE ONLY winds_data
    ADD CONSTRAINT "$1" FOREIGN KEY (sens_id) REFERENCES sensoren(id);


--
-- TOC entry 35 (OID 18433908)
-- Name: $1; Type: FK CONSTRAINT; Schema: public; Owner: losinshi
--

ALTER TABLE ONLY innen_data
    ADD CONSTRAINT "$1" FOREIGN KEY (sens_id) REFERENCES sensoren(id);


--
-- TOC entry 36 (OID 18433928)
-- Name: $1; Type: FK CONSTRAINT; Schema: public; Owner: losinshi
--

ALTER TABLE ONLY pyano_data
    ADD CONSTRAINT "$1" FOREIGN KEY (sens_id) REFERENCES sensoren(id);


--
-- TOC entry 37 (OID 18433936)
-- Name: $1; Type: FK CONSTRAINT; Schema: public; Owner: losinshi
--

ALTER TABLE ONLY helli_data
    ADD CONSTRAINT "$1" FOREIGN KEY (sens_id) REFERENCES sensoren(id);


SET SESSION AUTHORIZATION 'postgres';

--
-- TOC entry 3 (OID 2200)
-- Name: SCHEMA public; Type: COMMENT; Schema: -; Owner: postgres
--

COMMENT ON SCHEMA public IS 'Standard public schema';


SET SESSION AUTHORIZATION 'losinshi';

--
-- TOC entry 29 (OID 18433947)
-- Name: FUNCTION get_sensor_id(integer, integer); Type: COMMENT; Schema: public; Owner: losinshi
--

COMMENT ON FUNCTION get_sensor_id(integer, integer) IS 'erster Integer ist der Typ, 
2. Integer ist die Addresse. 
Rueckgabe ist die passende id in der sensoren-tabelle.';


