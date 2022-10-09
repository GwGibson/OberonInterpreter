(*s-*)
MODULE test11;

	PROCEDURE test(a : INTEGER) : INTEGER;
		VAR b : INTEGER;
		
		PROCEDURE test2(p : INTEGER);
			
			PROCEDURE test3(p : INTEGER);
			
			BEGIN (* test3 *)
				WRITE(p + 1);
			END test3;
			
		BEGIN (* test2 *)
			WRITE(p * 2 + 4); 
			test3(p)
		END test2;
			
	BEGIN (* test *)
		b := 2;
		test2(b * a);
		b := a * 3 + 2;
		RETURN b
	END test;
		
BEGIN
	WRITE(test(5));
	(* EXPECTED OUTPUT: 24 11 17 *)
END test11.

