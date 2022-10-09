(*s-*)
MODULE test2;

VAR a : BOOLEAN;

BEGIN
	a := TRUE;
	IF a = FALSE THEN
		WRITE(7)
	ELSIF a = TRUE THEN
		WRITE(8);
	END
END test2.
