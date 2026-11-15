/* An array declared with its size left out, then defined with it: the
   two types are compatible, 6.2.7p1, and their composite is the
   array's, which rcc does not build, so the types conflict. */
extern int table[];
int table[3];

int
main(void)
{
        return 0;
}
