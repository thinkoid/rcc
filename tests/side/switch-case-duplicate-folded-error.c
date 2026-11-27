/* Two cases of one value after the fold. */
int
f(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 3:
        case 1 + 2: c = 1;
        }
        return c;
}
