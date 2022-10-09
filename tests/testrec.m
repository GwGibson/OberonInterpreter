(*s-*)
MODULE testrec;
	VAR
		b, e, r : INTEGER;
		
	(* Computes m ^ n recursively *)
	PROCEDURE exp(m, n : INTEGER) : INTEGER;
	VAR
		f : INTEGER;
	BEGIN
		IF n = 0 THEN
			f := 1
		ELSE
			f := m * exp(m, n - 1)
		END;
		RETURN f
	END exp;


BEGIN
	READ(b);
	READ(e);
	r := exp(b, e);
	WRITE(r)

END testrec.
