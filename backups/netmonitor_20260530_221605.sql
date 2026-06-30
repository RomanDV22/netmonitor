--
-- PostgreSQL database dump
--

\restrict WclG9vI22vulvfSP21oFqLD2pA5Bl4VgGYrjAY5ikJpgiMzsf6LWK4k8wXrhFwr

-- Dumped from database version 16.14 (Ubuntu 16.14-0ubuntu0.24.04.1)
-- Dumped by pg_dump version 16.14 (Ubuntu 16.14-0ubuntu0.24.04.1)

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: dispositivos; Type: TABLE; Schema: public; Owner: claude
--

CREATE TABLE public.dispositivos (
    id integer NOT NULL,
    nombre character varying(100) NOT NULL,
    ip character varying(15),
    mac character varying(17),
    tipo character varying(50),
    activo boolean DEFAULT true,
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.dispositivos OWNER TO claude;

--
-- Name: dispositivos_id_seq; Type: SEQUENCE; Schema: public; Owner: claude
--

CREATE SEQUENCE public.dispositivos_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.dispositivos_id_seq OWNER TO claude;

--
-- Name: dispositivos_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: claude
--

ALTER SEQUENCE public.dispositivos_id_seq OWNED BY public.dispositivos.id;


--
-- Name: interfaces; Type: TABLE; Schema: public; Owner: claude
--

CREATE TABLE public.interfaces (
    id integer NOT NULL,
    dispositivo_id integer,
    nombre character varying(50) NOT NULL,
    ip character varying(15),
    mac character varying(17),
    tipo character varying(20),
    activa boolean DEFAULT true
);


ALTER TABLE public.interfaces OWNER TO claude;

--
-- Name: interfaces_id_seq; Type: SEQUENCE; Schema: public; Owner: claude
--

CREATE SEQUENCE public.interfaces_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.interfaces_id_seq OWNER TO claude;

--
-- Name: interfaces_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: claude
--

ALTER SEQUENCE public.interfaces_id_seq OWNED BY public.interfaces.id;


--
-- Name: trafico; Type: TABLE; Schema: public; Owner: claude
--

CREATE TABLE public.trafico (
    id integer NOT NULL,
    interfaz_id integer,
    "timestamp" timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    protocolo character varying(10),
    ip_origen character varying(15),
    ip_destino character varying(15),
    puerto_origen integer,
    puerto_destino integer,
    tamanio integer,
    info text
);


ALTER TABLE public.trafico OWNER TO claude;

--
-- Name: trafico_id_seq; Type: SEQUENCE; Schema: public; Owner: claude
--

CREATE SEQUENCE public.trafico_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.trafico_id_seq OWNER TO claude;

--
-- Name: trafico_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: claude
--

ALTER SEQUENCE public.trafico_id_seq OWNED BY public.trafico.id;


--
-- Name: dispositivos id; Type: DEFAULT; Schema: public; Owner: claude
--

ALTER TABLE ONLY public.dispositivos ALTER COLUMN id SET DEFAULT nextval('public.dispositivos_id_seq'::regclass);


--
-- Name: interfaces id; Type: DEFAULT; Schema: public; Owner: claude
--

ALTER TABLE ONLY public.interfaces ALTER COLUMN id SET DEFAULT nextval('public.interfaces_id_seq'::regclass);


--
-- Name: trafico id; Type: DEFAULT; Schema: public; Owner: claude
--

ALTER TABLE ONLY public.trafico ALTER COLUMN id SET DEFAULT nextval('public.trafico_id_seq'::regclass);


--
-- Data for Name: dispositivos; Type: TABLE DATA; Schema: public; Owner: claude
--

COPY public.dispositivos (id, nombre, ip, mac, tipo, activo, created_at) FROM stdin;
3	Router-Casa	192.168.0.1	CC:DD:EE:FF:00:11	router	t	2026-05-23 18:26:05.000326
1	Roman-PC2026	192.168.0.139	AA:BB:CC:DD:EE:FF	pc	t	2026-05-23 18:00:30.820004
2	Notebook	192.168.0.118	AA:BB:CC:DD:EE:FE	notebook	t	2026-05-23 18:00:41.130582
5	192.168.0.33	desconocido	\N	desconocido	t	2026-05-23 21:54:42.164228
6	192.168.0.123	desconocido	\N	desconocido	t	2026-05-23 21:56:15.405721
7	192.168.0.124	desconocido	\N	desconocido	t	2026-05-23 21:56:15.431972
11	192.168.0.126	desconocido	\N	desconocido	t	2026-05-23 22:05:13.53466
12	192.168.0.161	desconocido	\N	desconocido	t	2026-05-23 22:05:49.867606
\.


--
-- Data for Name: interfaces; Type: TABLE DATA; Schema: public; Owner: claude
--

COPY public.interfaces (id, dispositivo_id, nombre, ip, mac, tipo, activa) FROM stdin;
\.


--
-- Data for Name: trafico; Type: TABLE DATA; Schema: public; Owner: claude
--

COPY public.trafico (id, interfaz_id, "timestamp", protocolo, ip_origen, ip_destino, puerto_origen, puerto_destino, tamanio, info) FROM stdin;
1	\N	2026-05-23 20:48:36.540902	ICMP	172.28.37.80	192.168.0.1	0	0	\N	\N
2	\N	2026-05-23 20:48:36.59454	ICMP	192.168.0.1	172.28.37.80	0	0	\N	\N
3	\N	2026-05-23 20:48:37.564175	ICMP	172.28.37.80	192.168.0.1	0	0	\N	\N
4	\N	2026-05-23 20:48:37.578535	ICMP	192.168.0.1	172.28.37.80	0	0	\N	\N
5	\N	2026-05-23 20:48:38.588256	ICMP	172.28.37.80	192.168.0.1	0	0	\N	\N
6	\N	2026-05-23 20:48:38.59301	ICMP	192.168.0.1	172.28.37.80	0	0	\N	\N
7	\N	2026-05-23 20:48:39.612285	ICMP	172.28.37.80	192.168.0.1	0	0	\N	\N
8	\N	2026-05-23 20:48:39.620172	ICMP	192.168.0.1	172.28.37.80	0	0	\N	\N
9	\N	2026-05-23 20:48:40.636208	ICMP	172.28.37.80	192.168.0.1	0	0	\N	\N
10	\N	2026-05-23 20:48:40.64349	ICMP	192.168.0.1	172.28.37.80	0	0	\N	\N
11	\N	2026-05-23 20:48:41.660164	ICMP	172.28.37.80	192.168.0.1	0	0	\N	\N
12	\N	2026-05-23 20:48:41.66425	ICMP	192.168.0.1	172.28.37.80	0	0	\N	\N
13	\N	2026-05-23 20:48:42.684261	ICMP	172.28.37.80	192.168.0.1	0	0	\N	\N
14	\N	2026-05-23 20:48:42.68838	ICMP	192.168.0.1	172.28.37.80	0	0	\N	\N
15	\N	2026-05-23 20:48:43.708348	ICMP	172.28.37.80	192.168.0.1	0	0	\N	\N
16	\N	2026-05-23 20:48:43.715647	ICMP	192.168.0.1	172.28.37.80	0	0	\N	\N
17	\N	2026-05-23 20:48:44.732273	ICMP	172.28.37.80	192.168.0.1	0	0	\N	\N
18	\N	2026-05-23 20:48:44.734338	ICMP	192.168.0.1	172.28.37.80	0	0	\N	\N
19	\N	2026-05-23 21:43:47.502965	UDP	172.28.37.80	91.189.91.157	46676	123	\N	\N
20	\N	2026-05-23 21:43:47.514464	UDP	91.189.91.157	172.28.37.80	123	46676	\N	\N
21	\N	2026-05-23 21:44:00.815568	ICMP	172.28.37.80	192.168.0.1	0	0	\N	\N
22	\N	2026-05-23 21:44:00.820793	ICMP	192.168.0.1	172.28.37.80	0	0	\N	\N
23	\N	2026-05-23 21:44:01.839364	ICMP	172.28.37.80	192.168.0.1	0	0	\N	\N
24	\N	2026-05-23 21:44:01.847995	ICMP	192.168.0.1	172.28.37.80	0	0	\N	\N
25	\N	2026-05-23 21:44:02.863358	ICMP	172.28.37.80	192.168.0.1	0	0	\N	\N
26	\N	2026-05-23 21:44:02.870905	ICMP	192.168.0.1	172.28.37.80	0	0	\N	\N
27	\N	2026-05-23 21:44:03.887535	ICMP	172.28.37.80	192.168.0.1	0	0	\N	\N
28	\N	2026-05-23 21:44:03.895931	ICMP	192.168.0.1	172.28.37.80	0	0	\N	\N
29	\N	2026-05-23 21:44:04.911007	ICMP	172.28.37.80	192.168.0.1	0	0	\N	\N
30	\N	2026-05-23 21:44:04.914566	ICMP	192.168.0.1	172.28.37.80	0	0	\N	\N
31	\N	2026-05-23 21:44:05.935128	ICMP	172.28.37.80	192.168.0.1	0	0	\N	\N
32	\N	2026-05-23 21:44:05.942005	ICMP	192.168.0.1	172.28.37.80	0	0	\N	\N
33	\N	2026-05-23 21:44:06.959211	ICMP	172.28.37.80	192.168.0.1	0	0	\N	\N
34	\N	2026-05-23 21:44:06.968397	ICMP	192.168.0.1	172.28.37.80	0	0	\N	\N
\.


--
-- Name: dispositivos_id_seq; Type: SEQUENCE SET; Schema: public; Owner: claude
--

SELECT pg_catalog.setval('public.dispositivos_id_seq', 12, true);


--
-- Name: interfaces_id_seq; Type: SEQUENCE SET; Schema: public; Owner: claude
--

SELECT pg_catalog.setval('public.interfaces_id_seq', 1, false);


--
-- Name: trafico_id_seq; Type: SEQUENCE SET; Schema: public; Owner: claude
--

SELECT pg_catalog.setval('public.trafico_id_seq', 34, true);


--
-- Name: dispositivos dispositivos_pkey; Type: CONSTRAINT; Schema: public; Owner: claude
--

ALTER TABLE ONLY public.dispositivos
    ADD CONSTRAINT dispositivos_pkey PRIMARY KEY (id);


--
-- Name: interfaces interfaces_pkey; Type: CONSTRAINT; Schema: public; Owner: claude
--

ALTER TABLE ONLY public.interfaces
    ADD CONSTRAINT interfaces_pkey PRIMARY KEY (id);


--
-- Name: trafico trafico_pkey; Type: CONSTRAINT; Schema: public; Owner: claude
--

ALTER TABLE ONLY public.trafico
    ADD CONSTRAINT trafico_pkey PRIMARY KEY (id);


--
-- Name: interfaces interfaces_dispositivo_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: claude
--

ALTER TABLE ONLY public.interfaces
    ADD CONSTRAINT interfaces_dispositivo_id_fkey FOREIGN KEY (dispositivo_id) REFERENCES public.dispositivos(id);


--
-- Name: trafico trafico_interfaz_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: claude
--

ALTER TABLE ONLY public.trafico
    ADD CONSTRAINT trafico_interfaz_id_fkey FOREIGN KEY (interfaz_id) REFERENCES public.interfaces(id);


--
-- PostgreSQL database dump complete
--

\unrestrict WclG9vI22vulvfSP21oFqLD2pA5Bl4VgGYrjAY5ikJpgiMzsf6LWK4k8wXrhFwr

