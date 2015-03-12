```sql
-- Table: apks

-- DROP TABLE apks;

CREATE TABLE apks
(
  id serial NOT NULL,
  name character varying(60) NOT NULL,
  system character varying(60) NOT NULL,
  version integer NOT NULL,
  url character varying(200),
  description text,
  created timestamp without time zone NOT NULL DEFAULT now(),
  CONSTRAINT apks_id_pkey PRIMARY KEY (id)
);
```
