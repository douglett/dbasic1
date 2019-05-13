(module $dbasic_program
	(func $foo (export "foo") (result i32)
		(local $a i32)
		(local.set $a (i32.const 10))
		(loop $loop1_top (block $loop1_end 
			(if (i32.lt_s (local.get $a) (i32.const 10)) (block 
				(local.set $a (i32.add (local.get $a) (i32.const 1)))
				(br $loop1_top)
			))
		))
		(return (local.get $a))
	)
)