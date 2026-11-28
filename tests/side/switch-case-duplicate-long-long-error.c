/* Two cases of one value of a long long switch, -1 and the unsigned
   constant of 64 ones converted, 6.8.5.3p5: rcc names it signed. */
int
f(void)
{
        long long x;
        switch (x) {
        case -1: return 1;
        case 0xffffffffffffffff: return 2;
        }
        return 0;
}
