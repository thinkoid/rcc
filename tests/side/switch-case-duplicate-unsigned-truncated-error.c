/* Two cases of one value of an unsigned switch, 0 and the constant whose
   low 32 bits are 0, 6.8.5.3p5. */
int
f(void)
{
        unsigned x;
        switch (x) {
        case 0: return 1;
        case 0xffffffff00000000: return 2;
        }
        return 0;
}
