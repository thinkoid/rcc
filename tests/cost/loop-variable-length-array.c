/* shape.md chapter 5, backend.md 7.2 and aarch64.md 7.2, a variable
   length array declared in the body of a loop, whose declaration is
   reached again each time around.  The end of the body is the end of
   the array's scope, so the stack pointer goes back there, before the
   jump back, to where the declaration kept it, and each turn takes the
   storage at the same address. */
int
f(int n)
{
        int k = 0;

        while (k < n) {
                char buf[n];

                buf[0] = 1;
                k += buf[0];
        }
        return k;
}

int
main(void)
{
        return f(1) != 1;
}
