/* Two cases of one value of a long switch, 1 and 1L, 6.8.5.3p5. */
int
f(void)
{
        long x;
        switch (x) {
        case 1: return 1;
        case 1L: return 2;
        }
        return 0;
}
