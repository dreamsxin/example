```sql
CREATE OR REPLACE FUNCTION plpgsql_call_handler() RETURNS language_handler AS
'$libdir/plpgsql' LANGUAGE C;

CREATE OR REPLACE FUNCTION plpgsql_validator(oid) RETURNS void AS
'$libdir/plpgsql' LANGUAGE C;

CREATE TRUSTED PROCEDURAL LANGUAGE plpgsql
HANDLER plpgsql_call_handler
VALIDATOR plpgsql_validator;
```

```sql
CREATE OR REPLACE FUNCTION catalog.path(int, int) RETURNS SETOF catalog.tree AS $tree_path$
DECLARE
f_lft INTEGER;
f_rgt INTEGER;
l_lft INTEGER;
l_rgt INTEGER;
rec catalog.tree;


BEGIN
SELECT lft, rgt INTO f_lft, f_rgt FROM catalog.tree WHERE id=$1;
RAISE NOTICE 'f_lft=%, f_rgt=%', f_lft, f_rgt;
SELECT lft, rgt INTO l_lft, l_rgt FROM catalog.tree WHERE id=$2;
RAISE NOTICE 'l_lft=%, l_rgt=%', l_lft, l_rgt;

FOR rec IN 
		SELECT id, sid, name, lft, rgt FROM catalog.tree WHERE lft BETWEEN f_lft AND l_lft AND	rgt BETWEEN l_rgt AND f_rgt ORDER BY lft
		LOOP
		RETURN NEXT rec;
END LOOP;
RETURN;							
END;				
$tree_path$ LANGUAGE plpgsql;
```

```sql
CREATE SCHEMA catalog;

--catalog_toc_table
CREATE TABLE catalog.tree (
id SERIAL PRIMARY KEY, 
sid integer, 
name varchar(300)
	CONSTRAINT not_empty_name CHECK(name <> ''),
lft integer, 
rgt integer)
	WITH OIDS;
	
ALTER SEQUENCE catalog.tree_id_seq MINVALUE 0 RESTART WITH 0;
INSERT INTO catalog.tree (sid, name, lft, rgt) values (NULL, 'root',1,2);


--trigger_status_table
CREATE TABLE catalog.trigger_status(status integer);
INSERT INTO catalog.trigger_status (status) values(1);
```

```sql
--function creation
CREATE OR REPLACE FUNCTION catalog.tree_error() RETURNS TRIGGER AS $tree_error$
DECLARE	
trigger_status INTEGER; --selfcall protection
		
BEGIN

--selfcall protection
SELECT status INTO trigger_status FROM catalog.trigger_status;
IF(trigger_status = 1) THEN
---------------------------------------------------------------

-------------------------INSERT---------------
	IF (TG_OP = 'INSERT') THEN
		IF(NEW.sid is NULL) THEN
		RAISE EXCEPTION 'sid cant be NULL!';
		END IF;		
		
-------------------------DELETE---------------
	ELSIF (TG_OP = 'DELETE') THEN
		IF(OLD.id = 0) THEN
		RAISE EXCEPTION 'dont delete root node!';
		END IF;		
										   			
-------------------------UPDATE----------------
	ELSIF (TG_OP = 'UPDATE') THEN
		IF(OLD.id <> NEW.id)THEN 
		RAISE EXCEPTION 'dont change id column!';
		ELSIF(OLD.lft <> NEW.lft) THEN
		RAISE EXCEPTION 'dont change lft column!';
		ELSIF(OLD.rgt <> NEW.rgt) THEN
		RAISE EXCEPTION 'dont change rgt column!';
		END IF;
	
	END IF;
----------------------------------------------------------------------------------------------

END IF;
-------------------------------------------------

RETURN NULL;
END;
$tree_error$ language plpgsql;

--trigger installation
CREATE TRIGGER tree_error
AFTER INSERT OR UPDATE OR DELETE ON catalog.tree
FOR EACH ROW EXECUTE PROCEDURE catalog.tree_error();
```

```sql
	--function creation
CREATE OR REPLACE FUNCTION catalog.tree_edit() RETURNS TRIGGER AS $tree_edit$
DECLARE
	trigger_status INTEGER; --selfcall protection
	var_rgt	INTEGER; --insert
	
	var_new_sid_rgt INTEGER; --update new parent old rgt, lft of child 
	var_move_size INTEGER; --update
	var_shift INTEGER; --update
	var_mid_lft INTEGER; --update
	var_mid_rgt INTEGER; --update
	
		
BEGIN

--selfcall protection
SELECT status INTO trigger_status FROM catalog.trigger_status;
IF(trigger_status = 1) THEN
UPDATE catalog.trigger_status SET status=0;
---------------------------------------------------------------


-------------------------INSERT---------------
	IF (TG_OP = 'INSERT') THEN
		SELECT rgt INTO var_rgt FROM catalog.tree WHERE id = NEW.sid;
		UPDATE catalog.tree SET lft = CASE WHEN lft>var_rgt
													THEN lft+2
													ELSE lft END,
										  rgt = CASE WHEN rgt >= var_rgt
										  			THEN rgt+2
										  			ELSE rgt END;
		UPDATE catalog.tree SET lft = var_rgt, rgt = (var_rgt+1) WHERE id=NEW.id; 
		
		
-------------------------DELETE---------------
	ELSIF (TG_OP = 'DELETE') THEN
		DELETE FROM catalog.tree WHERE lft BETWEEN OLD.lft AND OLD.rgt;
		UPDATE catalog.tree SET lft = CASE WHEN lft > OLD.lft
													THEN lft - (OLD.rgt - OLD.lft + 1)
													ELSE lft END,
										   rgt = CASE WHEN rgt > OLD.lft
										   			THEN rgt - (OLD.rgt - OLD.lft + 1)
										   			ELSE rgt END;
										   			
-------------------------UPDATE----------------
	ELSIF (TG_OP = 'UPDATE') THEN
		IF(OLD.sid <> NEW.sid) THEN
				
		SELECT rgt INTO var_new_sid_rgt FROM catalog.tree WHERE id=NEW.sid;
		var_move_size := OLD.rgt - OLD.lft + 1;
		
		UPDATE catalog.tree SET lft = CASE WHEN lft > var_new_sid_rgt
													THEN lft + var_move_size
													ELSE lft END,
								rgt = CASE WHEN rgt >= var_new_sid_rgt
													THEN rgt + var_move_size
													ELSE rgt END;
		
		SELECT lft, rgt INTO var_mid_lft, var_mid_rgt FROM catalog.tree WHERE id=OLD.id;											
		var_shift := var_new_sid_rgt - var_mid_lft;
		
		UPDATE catalog.tree SET lft = lft + var_shift, rgt = rgt +  var_shift WHERE lft BETWEEN var_mid_lft AND var_mid_rgt;
		
		UPDATE catalog.tree SET lft = CASE WHEN lft > var_mid_lft
													THEN lft - var_move_size
													ELSE lft END,
								rgt = CASE WHEN rgt > var_mid_lft
										   			THEN rgt - var_move_size
										   			ELSE rgt END;
		END IF;
END IF;
----------------------------------------------------------------------------------------------

--selfcall protection
UPDATE catalog.trigger_status SET status=1;
END IF;
-------------------------------------------------

RETURN NULL;
END;
$tree_edit$ language plpgsql;

--trigger installation
CREATE TRIGGER tree_edit
AFTER INSERT OR UPDATE OR DELETE ON catalog.tree
FOR EACH ROW EXECUTE PROCEDURE catalog.tree_edit();
```

```sql
DELETE FROM catalog.tree WHERE id<>0;
UPDATE catalog.tree SET lft=1, rgt=2;

ALTER SEQUENCE catalog.tree_id_seq MINVALUE 0 RESTART WITH 1;

--test table
INSERT INTO catalog.tree (name,sid) values('products',0); --1
INSERT INTO catalog.tree (name,sid) values('lib',0); --2
INSERT INTO catalog.tree (name,sid) values('vacancy',0); --3
INSERT INTO catalog.tree (name,sid) values('auto',1); --4
INSERT INTO catalog.tree (name,sid) values('vent',1); --5
INSERT INTO catalog.tree (name,sid) values('need',3); --6 
INSERT INTO catalog.tree (name,sid) values('LG',4); --7
INSERT INTO catalog.tree (name,sid) values('meng',6); --8
```