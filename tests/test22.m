(*s-*)
MODULE test22;
	VAR
		arr : ARRAY 5,2 OF INTEGER;
		bewl : ARRAY 4 OF BOOLEAN;
	
		test : RECORD
			x : INTEGER;
			b : ARRAY 3 OF BOOLEAN;
			a : ARRAY 5 OF INTEGER;
			y : INTEGER;
			bool : BOOLEAN
		END;

		final : BOOLEAN;

BEGIN	
	test.a[0] := 1;
	test.a[1] := test.a[0] + 1;
	test.a[3] := 4;
	
	test.x := 8;
	test.y := test.x * test.a[1];
	
	test.bool := TRUE;
	test.b[2] := test.bool;
	
	IF test.bool & (test.x = 8) THEN
		final := TRUE
	END;
	
	arr[3,2] := 17;
	
	test.a[2] := arr[3,2];
	
	IF (arr[3,2] = 17) & test.b[2] THEN
		final := TRUE
	ELSE
		final := FALSE
	END;
	
	bewl[0] := TRUE;
	bewl[1] := FALSE;
	
	IF ~bewl[1] = bewl[0] THEN
		final := TRUE
	ELSE
		final := FALSE
	END;
	
	WRITE(final);
	
	(* Expected output: 1 *)

END test22.
