'dim b = 1

'a function
function f()
	dim a = 10 + 10 * 5 - 58 'should be 1
	dim i
	dim j
	if a >= 0 then
		if a == 1 then
			return 1
		end if
	end if

	a = a + 1
	a=1
	a++
	i=1
	a--
	a += 20

	while a > 0 do
		return a
	end while

	for j = 1 to 20
		return 20
	next j
end function


function f2()
	dim a = 10 + 10 * 5 - 59 'should be 1
	dim i
	dim j

	return 20
end function