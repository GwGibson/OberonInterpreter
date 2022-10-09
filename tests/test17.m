(*s-*)
MODULE test17;
	VAR
		a : INTEGER;
		
	PROCEDURE double(VAR x : INTEGER);
	BEGIN
		x := x * 2
	END double;
	
	PROCEDURE fiveProc : INTEGER;
	BEGIN
		RETURN 5
	END fiveProc;
	
BEGIN
	a := fiveProc;
	a := a + fiveProc();
	double(a);
	WRITE(a);
	
	IF a = 20 THEN
		WRITE(7)
	ELSE
		WRITE(a)
	END;
	
	(* Expected output: 20 7 *)
	
END test17.

