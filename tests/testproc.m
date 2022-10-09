(*s-*)
MODULE testproc;
	VAR 
		x, y : INTEGER;
		b: BOOLEAN;
	
	PROCEDURE test(VAR m, n : INTEGER; z : BOOLEAN) : BOOLEAN;
	BEGIN
		m := m + 5;
		WRITE(m * m);
		RETURN z
	END test;

BEGIN
	x := 5;
	y := 6;
	b := TRUE;
	IF test(x, y, b) THEN
		WRITE(x * y)
	END
	
	(* Expected output: 100 60 *)
END testproc.
