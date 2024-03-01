```sql
CREATE EXTENSION IF NOT EXISTS pg_trgm CASCADE;

CREATE SEQUENCE tg_fingerprints_id_seq CYCLE;
DROP TABLE IF EXISTS tg_fingerprints CASCADE;
CREATE TABLE tg_fingerprints (
	id INT4 PRIMARY KEY DEFAULT nextval('tg_fingerprints_id_seq'),
	ip inet NOT NULL,  -- IP
	ua CHARACTER VARYING(1024) NOT NULL,  -- User-Agent
	fingerprints JSONB, -- 指纹信息
	created TIMESTAMPTZ DEFAULT now() NOT NULL	-- 创建时间
);

CREATE INDEX tg_fingerprints_ip_index ON tg_fingerprints USING BTREE (ip);
CREATE INDEX tg_fingerprints_ua_index ON tg_fingerprints USING BTREE (ua);
CREATE INDEX tg_fingerprints_fingerprints_index ON tg_fingerprints USING GIN (fingerprints);

-- SELECT create_hypertable('tg_fingerprints', 'created', if_not_exists => TRUE);
-- SELECT * FROM chunk_relation_size('tg_fingerprints');

COMMENT ON COLUMN public.tg_fingerprints.ip IS 'IP';
COMMENT ON COLUMN public.tg_fingerprints.ua IS 'User-Agent';

-- SELECT similarity(fingerprints::text, '{"ip": "ut exercitation aliqua esse incididxxx", "intl": "eiusmod xxx", "math": "veniam sint xxx", "audio": "nulla voluptate xxx", "fonts": "mollit voluptatexxx", "webGL": "ut xxx", "canvas": "", "screen": "voluptate esse xxx", "domRect": "Duis veniam mollit xxx", "svgRect": "eiusmod ea fugiat", "timezone": "enim ex proxxx", "userAgent": "sunt"}') FROM tg_fingerprints ORDER BY created LIMIT 100;
```
