(*s-*)
MODULE testgcd;
	VAR
		a, b : INTEGER;

		PROCEDURE gcd(x, y : INTEGER) : INTEGER;
		BEGIN
			WHILE x # y DO
				IF x > y THEN
					x := x - y
				ELSE
					y := y - x
				END
			END;
			RETURN x
		END gcd;

BEGIN
	READ(a);
	READ(b);
	WRITE(gcd(a, b));
END testgcd.
