#unsafe
dim mem = malloc(20)

function main()
	dim a = 1
	'asd
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
	dim acc = peek(mem, 0)
	acc += 3
	poke(mem, 0, acc)
	return acc
end function