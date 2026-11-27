/* The first case's line, not its column, in the error of the second. */
int
f(int a)
{
        int c = 0;
        switch (a) {
        case 1:
        case 2:
        case 1: c = 1;
        }
        return c;
}
