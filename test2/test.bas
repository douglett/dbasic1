dim mem = alloc(20, 25*2)
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
	acc++
	return acc
end function