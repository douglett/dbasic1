function main()
	dim a = 1
	dim butt = 4 * 4
	a += 2
	a++
	fn()
	butt = fn()
	if a == 1 then
		a ++
	end if
	return a
end function

function fn()
	return 100
end function