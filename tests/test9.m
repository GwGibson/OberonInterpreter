(*s-*)
MODULE test9;
	VAR a : INTEGER;
	
	PROCEDURE test : INTEGER;
		VAR b : INTEGER;
		BEGIN
			b := 7;
			b := b * 2;
			RETURN b
		END test;
		
	PROCEDURE test2(p : INTEGER) : INTEGER;
		BEGIN
			RETURN p * 2;
			WRITE(342);
		END test2;

BEGIN
	WRITE(test2(4) + test());
	WRITE(test2(test()));
	a := test() * test2(5);
	WRITE(a + test());
	(* EXPECTED OUTPUT: 22 28 154 *)
END test9.

