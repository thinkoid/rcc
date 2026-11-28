/* The third operand of ?: is a conditional expression, 6.5.16p1, so a
   ? b : c = 1 assigns to the conditional, which is no lvalue, its
   footnote 110, said at the '='. */

int
main(void)
{
        int a;
        int b;
        int c;
        a ? b : c = 1;
        return a;
}
