(*s-*)
(* Error handling checker *)

MODULE teste;
	CONST
		A = 4 * 2;
	VAR
		a, b, c, d, i, a, j : INTEGER;
		z : BOOLEAN;
		arr : ARRAY 5 OF INTEGER;
		point : RECORD
			x : INTEGER;
			y : INTEGER
		END;
		
	PROCEDURE test(a : INTEGER) : INTEGER;
		VAR b, a : INTEGER;
	BEGIN (* test *)
		RETURN b;
		WRITE(77)
	END test7;

	PROCEDURE test2(a : INTEGER);
	BEGIN (* test2 *)
		RETURN 48
	END test;
	
	PROCEDURE test3(a : INTEGER) : INTEGER;
	BEGIN (* test3 *)
		WRITE(a)
	END test3;
	
	PROCEDURE test4(VAR a : INTEGER);
	BEGIN (* test4 *)
		WRITE(a)
	END test4;

BEGIN
	z := 7 * 2;

	b := FALSE;

  c := FALSE * 2;	
	
	f := 7;

	z := TRUE;

	d := test(z);
	
	i := test2();
	
	i := test2;
	
	A := A * 2;

	test();

	test(1, 2);
	
	ABS(TRUE);
	
	i := ABS(TRUE);
	
	ABS(-28 * 2 - 4);
	
	ODD(TRUE);
	
	i := ODD(TRUE);
	
	arr[-1] := 3;
	
	arr[5] := 3;
	
	point.z := 5;
	
	point.x := TRUE;
	
	IF 1 THEN
		WRITE(2)
	END;
	
	test3(z);
	
  test4(14);
	
	
	
	
END teste2.
