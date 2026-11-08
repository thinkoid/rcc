/* G5: an uninitialized register variable holds a value. x has no
   initializer; c1 gives it ebx, which nothing writes, and both
   operands of == are that one register, cmpl %ebx, %ebx. The compare
   is written and is true: exit status 1. clang -O2 reads x as undef,
   which may be any value at each read, and folds main to return 0. */
int main(void)
{
    register int x;

    return x == x;
}
