/* G7: pointer comparison is address comparison. llc gives a and b
   adjacent slots, so one of &a + 1 and &b + 1 is the address of the
   other local; one comparison holds and the exit status is 1, whichever
   slot is lower. clang -O2 folds an equality between two distinct
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
