function main()
	dim a = 1
	dim butt = 4 * 4
	a += 2
	a++
	fn()
	butt = fn()
	return a
end function

function fn()
	return 100
end function