/* A second default label in one switch, 6.8.5.3p3: an error, though the
   first is in a block of the body.  The message names the line of the
   first. */
int
main(void)
{
        int n = 1;

        switch (n) {
        case 1: {
        default: n = 2;
        }
        default: n = 3;
        }

        return n;
}
