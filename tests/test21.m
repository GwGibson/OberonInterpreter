(*s-*)
MODULE test21;
	VAR
		i : INTEGER;
		
		point : RECORD
			x : INTEGER;
			y : INTEGER;
			bool : BOOLEAN
		END;
		
		point2 : RECORD
			x : INTEGER;
			y : INTEGER;
			z : INTEGER
		END;
		
		a : BOOLEAN;
	
BEGIN	
	point.x := 3;
	point.y := 5;
	point.bool := TRUE;
	
	point2.x := 7 * 2;
	point2.y := 3;
	point2.z := 9;

	IF point.bool THEN
		WRITE(point2.x * point2.y)
	END;
	
	IF point2.z = point.y + 4 THEN
		WRITE(point2.z)
	END
	
	(* Expected output: 42 9 *)

END test21.
