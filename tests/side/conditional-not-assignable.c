/* A conditional is no lvalue, 6.5.16 and its footnote 110, so it is no
   assignment's left operand, said at the '='. */

int
main(void)
{
        int a;
        int b;
        int c;
        (a ? b : c) = 1;
        return a;
}
