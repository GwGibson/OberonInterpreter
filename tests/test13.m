(*s-*)
MODULE test13;
	TYPE
		Months = (Jan, Feb, Mar, Apr, May, June, July, Aug, Sept, Oct, Nov, Dec);
		
	VAR 
		i, j : INTEGER;
		b : BOOLEAN;

BEGIN
	i := 0;
	j := 5;
	b := TRUE;
	
	WHILE i < July DO
		WRITE(i);
		i := i + 1
	ELSIF j = June DO
		WRITE(50);
		j := 6
	ELSIF j = July DO
		WRITE(60);
		j := 7
	ELSIF b = ~ FALSE DO
		WRITE(99);
		b := FALSE
	END;
	
	WRITE(111)
	
	(* Expected output: 0 1 2 3 4 5 50 60 99 111 *)

END test13.


