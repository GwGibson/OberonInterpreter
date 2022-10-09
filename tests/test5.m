(*s-*)
MODULE test5;

VAR a, b, c : INTEGER;

BEGIN
	a := 5;
	b := 2;
	c := 3;
	WRITE(a + b + c - 11);
	WHILE c < 10 DO
		WRITE(c);
		c := c + 1;
	END 
END test5.

