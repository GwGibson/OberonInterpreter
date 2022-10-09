(*s-*)
MODULE test14;
	VAR
		i : INTEGER;

BEGIN
	
	READ(i);

	CASE i + 1 OF
			1,2,3,4: WRITE(1)
		| 5 + 7: WRITE(2)
		| 7..9: WRITE(3)
	ELSE
		WRITE(10)
	END;

END test14.
