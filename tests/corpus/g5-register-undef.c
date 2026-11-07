/* G5: an uninitialized register variable holds a value. x has no
   initializer; rcc gives it one slot nothing writes and reads it once,
   load and freeze, and that one value is both operands of ==. The
   compare is emitted, its right operand frozen again so the selector
   does not fold it, and is true: exit status 1. clang -O2 reads x as
   undef, which may be any value at each read, and folds main to
   return 0. */
int main(void)
{
    register int x;

    return x == x;
}
