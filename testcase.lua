--[[Obs! The output of this compiler is a .s assembly file not .cc. 


I made another testcase which uses both tables and function calls(not recursively in this case), aswell as some extra
things like scopes and multi dimensional arrays, you can also return functions or pass them as arguments. 

Also while it is possible to store strings in variables any operations on strings will probably break everything and if you
want to print a string in a variable you have to use the prints function or it will default to printing as a number. ]]


--[[function sort( ... )
	-- body
end

funcs.scopeName = sort 		-- Does not work. 
funcs[5] = sort 			-- Does work, but you need to first define funcs with the right size like, funcs = {[1337] = 0}. Because I couldn't be bothered to keep track of the size.
funcs = {scopeName = sort}	-- Does work.


]]


funcs = {

sort = function( l ) -- You can also store the function like this {[5] = function} then call it with funcs[5]()
	repeat
	  hasChanged = false
	  itemCount=itemCount - 1
	  for i = 1, itemCount do
	    if l[i] > l[i + 1] then
	      l[i], l[i + 1] = l[i + 1], l[i]
	      hasChanged = true
	    end
	  end
	until hasChanged == false

end}

list = { 5, 6, 1, 2, 9, 14, 2, 15, 6, 7, 8, 97 }
itemCount = #list --Don't overwrite list[0] that's where the size is saved.

funcs.sort(list)

for i = 1, itemCount do
  io.write (list[i], " ")
end

print("\n")

asd = 
{
	{1, 2},
	{3, 4, 5}
}

for i = 1, #asd do
	for j = 1, #asd[i] do
		print(asd[i][j], " ")
	end
	print "\n"
end