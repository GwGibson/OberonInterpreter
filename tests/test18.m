(*s-*)
MODULE test18;
	VAR
		i : INTEGER;
		
BEGIN
	REPEAT
		i := i + 1;
		WRITE(i);
		
		IF i = 3 THEN
			WRITE(9);
			EXIT
		END
	UNTIL i > 5;
	
END test18.

