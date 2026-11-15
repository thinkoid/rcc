/* Two cases of one switch with one value after the conversion to the
   promoted type of the controlling expression, 6.8.5.3p3, p5: an
   error.  4294967297 is a long, and 1 as an int.  The message names the
   value and the line of the first. */
int
main(void)
{
        int n = 1;

        switch (n) {
        case 1: return 1;
        case 4294967297: return 2;
        }

        return 0;
}
