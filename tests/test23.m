(*s-*)
MODULE test23;
	TYPE
		POINT = RECORD
			x : INTEGER;
			y : INTEGER;
		END;
		
		ARR = ARRAY 5 OF INTEGER;
		
	VAR
		ar, ar2 : ARR;
		p1 : POINT;
		
		point2 : RECORD
			x : INTEGER;
			a : ARR;
			y : INTEGER;
			bool : BOOLEAN;
		END;
		
BEGIN	
	ar[0] := 1;
	ar[1] := 5;
	
	p1.x := 5;
	p1.y := 6;

	point2.a[0] := p1.x * p1.x;
	point2.a[1] := p1.y * p1.y;
	
	point2.x := 10;
	point2.y := 10;
	
	ar2[0] := point2.x * p1.x * point2.a[0] * ar[0];
	ar2[1] := ar2[0] * 2;

	WRITE(ar2[0])
	
	(* Expected output: 1250 *)

	
END test23.
