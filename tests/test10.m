(*s-*)
MODULE test10;
	VAR a, b : INTEGER;

	PROCEDURE test2(VAR z : INTEGER; v : INTEGER; y : BOOLEAN);
		BEGIN
			IF y = TRUE THEN
				z := 5;
				v := v + 2;
				WRITE(v + 3);
				WRITE(z * 2)
			END
		END test2;

BEGIN
	a := 10;
	b := 2;
	test2(a, b, TRUE);
	WRITE(a);
	WRITE(b)

	(* EXPECTED OUTPUT: 7 10 5 2 *)
END test10.

