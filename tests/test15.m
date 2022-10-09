(*s-*)
MODULE testx;
	VAR
		a : INTEGER;

	PROCEDURE one(x : INTEGER) : INTEGER;
		VAR 
			y : INTEGER;

		PROCEDURE two() : INTEGER;

			PROCEDURE three() : INTEGER;
			BEGIN
				RETURN 5
			END three;
			
			PROCEDURE four() : INTEGER;
			BEGIN
				RETURN 4
			END four;
			
		BEGIN (* two *)
			RETURN 20 * three() * four()
		END two;

	BEGIN (* one *)
		y := 800;
		RETURN x * two() + y
	END one;
BEGIN
	a := one(3);
	WRITE(a);
	
	(* Expected output: 2000 *)
	
END testx.
