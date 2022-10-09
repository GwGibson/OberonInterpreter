(*s-*)
MODULE test7;

CONST A = 13;
VAR i : INTEGER; d : BOOLEAN;

BEGIN
	
	FOR i := 1 TO A DO
		WRITE(i);
	END;
	
	d := TRUE;
	
	IF ~d = FALSE THEN
		WRITE(A);
	END
	
	
	
END test7.

