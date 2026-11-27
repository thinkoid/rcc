/* The rules that steer a value around a call on x86-64, each with the
   program it differs from.  A call spills every held value first, so
   a + g(b) holds nothing in a register across it.  A call keeps its
   own pin, the result in rax, so it is generated outside the
   reservation around it: the argument of the divisor g(b + 1) is
   computed in eax, where the divisor b + 1 is computed in ecx, outside
   rax and rdx.  A call pins rax within a right operand, so the node
   names no register to its left operand: a + (b * g(c)) writes the
   product into eax, the call's register, and reads b and a in their
   slots, where a + (b * c) holds a in eax and the product out of it.
   A call pins nothing at the top of a right operand, so a + g(b / c)
   loads a into eax, where a + b / c loads it into ecx, and the
   division runs inside the argument with every register free.  The
   reservation of an outer division moves a value a call spills: in
   (a + g(b)) + c / a, a is loaded into ecx and the sum is not written
   into eax, which is left for last, so a is reloaded.  A division in
   a right operand keeps the left's sum out of rax, (a + g(b)) * (b + c
   / a), but a call there spills it and adds nothing, (a + g(b)) * (b +
   g(c)), which reads a in its slot as (a + g(b)) + c does.  A call's
   result in rax moves out of the way of a later division, movq %rax,
   %rsi. */
int g(int a);

int
spill(int a, int b)
{
        return a + g(b);
}

int
call_divisor(int a, int b)
{
        return a / g(b + 1);
}

int
divisor(int a, int b)
{
        return a / (b + 1);
}

int
call_within(int a, int b, int c)
{
        return a + (b * g(c));
}

int
no_call_within(int a, int b, int c)
{
        return a + (b * c);
}

int
argument_division(int a, int b, int c)
{
        return a + g(b / c);
}

int
top_division(int a, int b, int c)
{
        return a + b / c;
}

int
reserved_division(int a, int b, int c)
{
        return (a + g(b)) + c / a;
}

int
ahead_division(int a, int b, int c)
{
        return (a + g(b)) * (b + c / a);
}

int
ahead_call(int a, int b, int c)
{
        return (a + g(b)) * (b + g(c));
}

int
plain(int a, int b, int c)
{
        return (a + g(b)) + c;
}

int
division_after_call(int a, int b, int c)
{
        return a + (g(b) + c / a);
}
