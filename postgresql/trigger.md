```psql
CREATE OR REPLACE FUNCTION trigger_func_xxx() RETURNS trigger AS $$
BEGIN
	PERFORM 1 FROM xxx WHERE id = new.id;
	IF NEW.id THEN
		RETURN null;
	END IF;
	RETURN new;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trigger_xxx BEFORE INSERT ON xxx FOR EACH ROW EXECUTE PROCEDURE trigger_func_xxx();
```
