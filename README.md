# RB
A lightweight arbitrary-precision arithmetic library, partially-complete.

The ADD, SUB and MUL operators work with Integers (equivalently, the "wholepart" of a real-valued structure. Look in 'aplib.h' at 'typedef struct ap', it is self-evident. With the wholepart populated for operands A and B, a correct result C(->wholepart) is generated.

The rest of the functions are fun to read, and they compile, link and output a generated PE. (I'm using the gdb on-line debugger, so I haven't tested the Makefile.) They are not yet "semantically" bullet-proofed, in terms of correct behaviour running on a target CPU (and my bias here is totally WIN32-console target with gcc).

To clarify, I am currently completing the operational development of the DIV (divide, and thus BxN) operator, and then the (Newton's Method) Root and Log operators. Oh, they are fun. But not de-rigured.

You would compile the PE, and invoke with:

/> app.exe (mul|add|sub) num1 num2

num1 and num2 would be base10-digit (0..9) integer number strings. They will then produce correct results for ADD, SUB, MUL.

E.G.

/> app.exe mul 12 12 

This would print result 144, and exit.


DIV, BxN, and the others are still being moulded. (They are fun though, look at the LOGb operator, how wonderful, once I've tested it, and modified for any source string errors I'll find.)


