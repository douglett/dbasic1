'a function
function f()
	dim a = 10 + 10 * 5 - 59 'blah
	dim i
	if a >= 0 then
		if a == 1 then
			return 1
		end if
	end if

	while a > 0 do
		return 10
	end while

	for i = 1 to 20
		return 10
	next i
end function