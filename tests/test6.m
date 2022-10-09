(*s-*)
MODULE test6;

VAR a, b, c, i, j : INTEGER;

BEGIN
	READ(a, b);
	c := 3;
	
	FOR i := 1 TO a DO
		WRITE(i);
	END;
	
	i := i + 4 - 3 + b DIV 2 * 8;
	WRITE(i);
	
	IF i = 15 THEN
		WRITE(15)
	ELSIF i = 16 THEN
		WRITELN(16)
	ELSIF i = 17 THEN
		WRITE(17)
	ELSE
		WRITE(1)
	END;
	
	j := 1;
	
	REPEAT 
		WRITE(c + j);
		j := j + 1
	UNTIL j > 5;
	
	
	
END test6.

