/* A variable length array declared in the body of a loop, whose
   declaration is reached again each time around: rcc does not compile
   it yet. */
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
