# LUACompiler
A compiler written in C++ which takes LUA code and outputs assembly

The grammar supports the full LUA language. (I think)
The compiler does not support everything.

You will need flex, bison, and gcc. Graphviz if you want the tree view of the translation.

This has not been tested in about 2 years, but I belive it worked like this.
Build it with make
run comp Testcase.lua -output name(maybe)-
Then we cheat and use gcc to build the assembly into an executable
