(*s-*)
MODULE test3;

VAR a, c, f : INTEGER; b : BOOLEAN;

BEGIN
	b := TRUE;
	a := 6;
	f := 2;
	c := a + 8;
	WRITE(c + 5);
	IF b = TRUE THEN
		a := 7;
		WRITE(a)
	END;
	
	IF a < 5 THEN
		a := 14;
		WRITE(a);
	ELSIF a < 8 THEN
		c := 24;
		WRITE(c);
	END
	
	(* 
	Expected output: 19 7 24 
	Expected final stack: 0 0 0 7 24 2 1 
	*)
END test3.
