(*s-*)
(* If everything works as expected will output 1 *)

MODULE testm;
	CONST
		B = TRUE OR FALSE;
		X = 5 + 5;
		A = 10 * 2 + 4 - 9;
		C = TRUE & FALSE;
		D = 3;
		E = 10 * A + X;
		F = -2 * 3 - 10;
		G = 100 + E DIV 2;
	TYPE
		Months = (Jan, Feb, Mar, Apr, May, June, July, Aug, Sept, Oct, Nov, Dec);
	VAR
		a, b, c, d, i, j, k, r, p, q : INTEGER;
		z, m, n, final : BOOLEAN;
		e : ARRAY 5 OF INTEGER;
		point : RECORD
			x : INTEGER;
			y : INTEGER
		END;

	PROCEDURE exp(b, e : INTEGER) : INTEGER;
	VAR
		f : INTEGER;
	BEGIN
		IF e = 0 THEN
			f := 1
		ELSE
			f := b * exp(b, e - 1)
		END;
		RETURN f
	END exp;

	PROCEDURE mul3(a, b, c : INTEGER; d : BOOLEAN) : BOOLEAN;
	VAR x : BOOLEAN;
	BEGIN
		IF d = FALSE THEN
			x := FALSE
		ELSIF (a * b * c <= 20) & d THEN
			x := TRUE
		ELSE
			x := FALSE
		END;
		RETURN x
	END mul3;
	
	PROCEDURE test(a : INTEGER) : INTEGER;
		VAR b : INTEGER;
		
		PROCEDURE test2(VAR a : INTEGER);
			VAR p : BOOLEAN;
				
			PROCEDURE test3(VAR a : INTEGER);
			VAR b : BOOLEAN;
			BEGIN (* test3 *)
				b := FALSE;
				IF b = TRUE THEN
					a := a * 2 - 4 - 1
				ELSIF b = FALSE THEN
					a := a DIV 2 - 4 - 1
				END
			END test3;
			
		BEGIN (* test2 *)
			a := a * 3 + 3 - 8;
			test3(a)
		END test2;

		PROCEDURE test4 : INTEGER;
		BEGIN
			RETURN 3;
		END test4;

		PROCEDURE test5(a : INTEGER) : INTEGER;
		BEGIN
			RETURN a * 2
		END test5;
			
	BEGIN (* test *)
		b := 2;
		b := b * a * test4 DIV test5(1);
		test2(b);
		RETURN b;
		WRITE(77)
	END test;
	
	PROCEDURE double(VAR x : INTEGER);
	BEGIN
		x := x * 2
	END double;
	
	PROCEDURE fiveProc : INTEGER;
	BEGIN
		RETURN 5
	END fiveProc;

BEGIN (* testm *)
	(* mul3 isn't working? *)
	final := FALSE;
	z := TRUE;
	a := test(5);
  (* a = 15 *)
	
	IF ~ final THEN
		final := TRUE;
		double(a)
	ELSE
		final := FALSE
	END;
	(* a = 30 *)
	
	p := ABS(-20 * 2);
	IF p # 40 THEN
		final := FALSE
	END;
	
	IF ODD(17) # TRUE THEN
		final := FALSE
	ELSIF ODD(16) = TRUE THEN
		final := FALSE
	END;
	
	b := Apr;
	d := 4;

	IF m & n THEN
		final := FALSE
	END;

	IF ((X = 10) & (A >= 10) & C) OR C OR (A + X = 25) THEN
		m := TRUE
	ELSE
		final := FALSE
	END;
	
	k := fiveProc;
	k := k + fiveProc;
	double(k);
	IF k # 20 THEN
		final := FALSE
	END;
	
	(* a = 30, b = 3, d = 4 *)
	
	IF z = B THEN
		c := 1
	ELSIF b = Feb THEN
		c := 2
	ELSIF b = Mar THEN
		c := 3
	ELSIF b = Apr THEN
		c := 4
	ELSE
		c := fiveProc
	END;
	
	e[2] := 5;
	
	IF e[2] * 2 # 10 THEN
		final := FALSE
	END;
	
	IF d # b THEN
		d := 15
	END;
	
	r := exp(5, 3);
	IF r # 125 THEN
		final := FALSE
	END;

	(* a = 30, b = 3, c = 4, d = 15 *)
	
	WHILE c > 0 DO
		d := d + c;
		c := c - 1
	END;

	FOR q := 2 TO 7 BY 2 DO
		r := 77 + 4 - 2;
		k := 7 * 2
	END;
	
	IF q # 8 THEN
		final := FALSE
	END;

	(* a = 30, b = 3, c = 0, d = 25 *)
	FOR i := 1 TO d - 20 DO
		d := d + i
	END;
	
	REPEAT 
		c := c + d;
		d := d - 10
	UNTIL d <= 0;
	
	(* a = 30, b = 3, c = 100, d = 0 *)

	WHILE d < July DO
		d := d + 1
	ELSIF b = 3 DO
		b := Nov
	ELSIF z = TRUE DO
		z := FALSE
	END;
	
	(* a = 30, b = 10, c = 100, d = 6 *)
	
	CASE b + d OF
			1,2,3..4: 
				a := 1;
				d := 10
		| 9,10,11,12:
				a := 3;
				d := 30
		| 13..20:
				a := 4;
				d := 40
	ELSE
		final := FALSE
	END;
	
	point.x := 3;
	point.y := 5;
	
	IF point.x # point.y - 2 THEN
		final := FALSE
	END;
	
	j := (5 + 2 * ( 4 + 5 ) * 2 + 1) DIV 2;
	
	IF j # 21 THEN
		final := FALSE
	END;
	
	IF final = TRUE THEN
		WRITE(1)
	ELSE
		WRITE(0)
	END
	

	
END testm.
