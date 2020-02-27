dim mem = malloc(20, 25*2)
dim acc

function main()
	dim a = 1
	dim butt = 4 * 4
	a += 2
	a++
	fn()
	butt = fn()
	'if a == 1 then
	'	a ++
	'end if
	return a
end function

function fn()
	poke(mem, 0, peek(mem, 0) + 3)
	return peek(mem, 0)
end function