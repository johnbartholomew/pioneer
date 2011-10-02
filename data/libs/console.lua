
do
	local builtin_print = print
	print = function (...)
		-- emit to the console
		local t = {...}
		-- note: can't use pairs() or ipairs() here,
		-- because they skip nil values
		for i = 1,#t do
			t[i] = tostring(t[i])
		end
		Console.AddLine(table.concat(t, '\t'))

		-- forward to the builtin print as well
		builtin_print(...)
	end
end
