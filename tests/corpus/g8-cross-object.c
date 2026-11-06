/* G8: a load reads the bytes at the address. The address of b is
   computed from the address of a by the difference of the two, 3 is
   stored through it, and b read back is the 3 stored: exit status 3.
   clang -O2 takes a pointer computed from &a to reach a only, so the
   store cannot change b, and returns the 2 stored in b (probe 4). The
   difference is known only at run time, so nothing folds before the
   promise applies. */
int main(int argc)
{
    int a = 1;
    int b = 2;
    long d = (char *) &b - (char *) &a;
    char *pa = (char *) &a;

    *(int *) (pa + d) = 3;

    return b;
}
