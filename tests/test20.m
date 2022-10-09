(*s-*)
MODULE test20;
	VAR
		a : INTEGER;
		array : ARRAY 5 OF INTEGER;
		barr : ARRAY 3 OF BOOLEAN;
		
BEGIN
	
	array[0] := 5;
	array[3] := 13;
	
	barr[1] := TRUE;
	barr[2] := FALSE;
	
	IF barr[1] = FALSE THEN
		a := 50
	ELSIF barr[2] = FALSE THEN
		a := 60
	END;
	
	IF array[0] * 10 + 10 = a THEN
		WRITE(array[3])
	END;
	
	(* Expected output: 13 *)

END test20.
