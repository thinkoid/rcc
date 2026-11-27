/* GNU C's case range, which rcc does not compile, in rcc's words. */
int
f(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1 ... 3: c = 1;
        }
        return c;
}
