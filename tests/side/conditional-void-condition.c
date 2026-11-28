/* A conditional's first operand is read for its value, 6.5.16p2, so a
   void one is an error at the '?', before the arms are read. */
void v(void);

int
main(void)
{
        int a;
        int b;
        return v() ? a : b;
}
