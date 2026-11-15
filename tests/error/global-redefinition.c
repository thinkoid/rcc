/* Two definitions of one object with initializers: a program has one
   external definition of it, 6.9.1p5. */
int count = 1;
int count = 2;

int
main(void)
{
        return count;
}
