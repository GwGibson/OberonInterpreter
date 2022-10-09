MODULE testfac;
	VAR input : INTEGER;

	PROCEDURE fact(in : INTEGER) : INTEGER;
	VAR ret : INTEGER;
	BEGIN
		IF in = 1 THEN
			ret := 1
		ELSE
			ret := in * fact(in - 1)
		END;
		RETURN ret
	END fact;

BEGIN
	READ(input);
	WRITE(fact(input))
END testfac.
