'a function
function f()
	dim a = 10 + 10 * 5 - 59 'blah
	dim i
	if a >= 0 then
		if a == 1 then
			return 1
		end if
	end if

	a = a + 1
	a=1
	a++
	a--
	a += 20

	while a > 0 do
		return 10
	end while

	for j = 1 to 20
		return 10
	next j
end function