/* G5: an uninitialized register variable holds a value. x has no
   initializer and no slot; rcc reads it as one freeze i32 undef, the
   same value at both operands of ==, so the comparison is true: exit
   status 1. clang -O2 reads x as undef, which may be any value at
   each read, and folds main to return 0. The object rcc builds has no
   cmp: FastISel folds a compare of one value with itself to its
   answer, so the 1 is the fold's as much as the compare's. */
int main(void)
{
    register int x;

    return x == x;
}
