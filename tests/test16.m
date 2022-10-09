(*s-*)
MODULE test16;
	CONST
		X = 5 + 5;
		A = 10 * 2 + 4 - 9;
		B = TRUE OR FALSE;
		C = TRUE & FALSE;
		D = 3;
		E = 10 * A + X;
		F = -2 * 3 - 10;
		G = 100 + E DIV 2;
		
BEGIN
	IF B OR C THEN
		WRITE(1)
	END;
	
	IF X = 10 THEN
		WRITE(2)
	END;
	
	IF ((X = 10) & (A >= 10) & C) OR C OR (A + X = 25) THEN
		WRITE(3)
	END;
	
	(* Expected output: 1 2 3 *)
	
END test16.
