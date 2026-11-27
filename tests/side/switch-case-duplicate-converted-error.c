/* Two cases of one value after the conversion to the promoted type,
   6.8.5.3p5. */
int
f(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 0: return 1;
        case 0x100000000: return 2;
        }
        return 0;
}
