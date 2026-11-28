/* A conditional starts with its first operand, 6.5.16p1: a '?' first
   is a missing expression. */

int
main(void)
{
        int a;
        int b;
        int c;
        return ? a : b;
}
