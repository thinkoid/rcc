/* The unary arithmetic operators, 6.5.4.3, on int.  - and ~ of a local
   are its load, then the operation on the register in place, negl or
   notl, neg or mvn; of a constant they fold to the constant, as rcc
   folds them.  + is the operand's value and writes nothing of its
   own.

   negate(), complement() and plus() return each operator of a local;
   constants() returns - and ~ of constants, folded, with + over one;
   nested() nests the operators, - -x and -~x, and parenthesized()
   puts each under a parenthesis.  initialized() initializes locals
   from each operator, the folded constant 0 among them, which aarch64
   stores from wzr; discarded() evaluates each for its effect;
   assigned() assigns each, and returns an assignment of one. */
int
negate(void)
{
        int x = 5;
        return -x;
}

int
complement(void)
{
        int x = 5;
        return ~x;
}

int
plus(void)
{
        int x = 5;
        return +x;
}

int
constants(void)
{
        int x = -7;
        int y = ~7;
        int z = -~0;
        return +-+5;
}

int
nested(void)
{
        int x = 3;
        int y = - -x;
        return -~x;
}

int
parenthesized(void)
{
        int x = 3;
        int y = -(x);
        int z = ~(-(x));
        return +(-(~(x)));
}

int
initialized(void)
{
        int x = 9;
        int y = -x;
        int z = -0;
        int w = ~-1;
        return +x;
}

int
discarded(void)
{
        int x = 9;
        -x;
        ~x;
        +x;
        -5;
        ~0;
        return x;
}

int
assigned(void)
{
        int x = 9;
        int y;
        y = -x;
        y = ~x;
        y = +x;
        y = -1;
        return y = ~y;
}
