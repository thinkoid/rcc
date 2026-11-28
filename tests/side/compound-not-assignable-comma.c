/* A comma's value is no lvalue, 6.5.18p2 and its footnote 114, so it
   is no compound assignment's left operand. */
void v(void);

int
main(void)
{
        int a = 1;
        int b = 2;
        (a, b) += 1;
        return a;
}
