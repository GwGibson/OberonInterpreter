(*s-*)
MODULE test8;
	CONST A = 13;
	VAR a : INTEGER;

	PROCEDURE test;
		VAR p : INTEGER;
		BEGIN
			p := 2;
			WRITE(p)
		END test;
		
	PROCEDURE test2(z, w : INTEGER);
		VAR p : INTEGER;
		BEGIN
			p := 1;
			WRITE(z + w + p)
		END test2;
		
	PROCEDURE test3(z : INTEGER) : INTEGER;
		BEGIN
			RETURN z + 5
		END test3;

BEGIN
	test();
	test2(4, 5 - 2);
	a := test3(5);
	WRITE(a); 
	
	(* Expected output: 2 8 10 *)

END test8.

