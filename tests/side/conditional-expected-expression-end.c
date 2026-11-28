/* A conditional's third operand is a conditional expression, 6.5.16p1:
   a ';' there is its missing expression. */

int
main(void)
{
        int a;
        int b;
        int c;
        return a ? b : ;
}
