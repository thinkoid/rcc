/* Two cases of one value of an unsigned long switch, -1 and 64 ones,
   6.8.5.3p5: rcc names the value unsigned, 18446744073709551615. */
int
f(void)
{
        unsigned long x;
        switch (x) {
        case -1: return 1;
        case 0xffffffffffffffff: return 2;
        }
        return 0;
}
