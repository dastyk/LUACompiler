# LUACompiler
A compiler written in C++ which takes LUA code and outputs assembly

The grammar supports the full LUA language. (I think)
The compiler does not support everything.

You will need flex, bison, and gcc, graphviz, and evince.
This has not been tested in about 2 years, but I belive it worked like this.
* Build it with make
* run comp Testcase.lua
* Then we cheat and use gcc to build the assembly (target.s) into an executable

Those who know assembly and wants to kill me because the asm code looks --- keep in mind that I did this in the span of 2 weeks and I had never writen a single line of assembly.
