/* No two cases of a switch have one value, 6.8.5.3p3, said at the
   second with the first's line. */
int
f(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1:
        case 1: c = 1;
        }
        return c;
}
