/* A conditional is no lvalue, its footnote 110, so it is no compound
   assignment's left operand. */

int
main(void)
{
        int a;
        int b;
        int c;
        (a ? b : c) += 1;
        return a;
}
