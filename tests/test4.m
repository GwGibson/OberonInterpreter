(*s-*)
MODULE test4;

VAR a, b, c, d : INTEGER; e, f : BOOLEAN;

BEGIN
	a := 14;
	b := 2;
	c := -4;
	WRITE(a + c * 2 DIV 4);
	d := a - c;
	WRITE(d);
	e := TRUE;
	f := FALSE;
	IF e = FALSE THEN
		d := 500;
		WRITE(d)
	ELSIF f = TRUE THEN
		d := 600;
		WRITE(d)
	ELSIF f # TRUE THEN
		d := 650;
		WRITE(d)
	ELSE
		d := 700;
		WRITE(d);
	END
	
	(* 
	Expected output: 12 18 650 
	*)
END test4.
