/* G7: pointer comparison is address comparison. c1 gives a and b
   adjacent slots off the frame pointer, so one of &a + 1 and &b + 1 is
   the address of the other local; one comparison holds and the exit
   status is 1, whichever slot is lower. On x86-64 b's is, -12(%rbp)
   below a's -8(%rbp), and &b + 1 == &a holds: leaq -8(%rbp) twice,
   cmpq and sete. On aarch64 a's is, [x29, #20] below b's [x29, #24],
   and &a + 1 == &b holds: add x0, x29, #24 and add x1, x29, #24, cmp
   and cset w0, eq. clang -O2 folds an equality between two distinct
   locals to false, the one-past-the-end address included, and returns
   0. The addresses exist only at run time, so nothing but the fold
   decides the comparison. `<` between the two locals is not folded:
   clang compares the addresses too. */
int main(int argc)
{
    int a;
    int b;

    return (&a + 1 == &b) + (&b + 1 == &a);
}
