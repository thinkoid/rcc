/* Two cases of one value of an unsigned long switch, the bit 63 alone,
   the second a constant expression whose first token is its -:
   6.8.5.3p5, said at the -. */
int
f(void)
{
        unsigned long x;
        switch (x) {
        case 0x8000000000000000: return 1;
        case -0x7fffffffffffffff - 1: return 2;
        }
        return 0;
}
