/* Two cases of one value of an unsigned switch, -1 and 4294967295
   converted, 6.8.5.3p5: rcc names the value as the type holds it. */
int
f(void)
{
        unsigned x;
        switch (x) {
        case -1: return 1;
        case 4294967295: return 2;
        }
        return 0;
}
