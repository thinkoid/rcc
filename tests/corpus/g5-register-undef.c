/* G5: an uninitialized register variable holds a value. x has no
   initializer, and c1 writes nothing to its register; both operands
   of == are that one register, and the compare is written.

   On x86-64 x is ebx, and the listing shows cmpl %ebx, %ebx.

   On aarch64 x is w19, and the listing shows cmp w19, w19.

   The compare is true: exit status 1. clang -O2 reads x as undef,
   which may be any value at each read, and folds main to return 0. */
int main(void)
{
    register int x;

    return x == x;
}
