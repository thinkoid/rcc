/* The first default's line in the error of the second. */
int
f(int a)
{
        int c = 0;
        switch (a) {
        default: c = 1;
        case 1:
        default: c = 2;
        }
        return c;
}
