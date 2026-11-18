/* A conditional of two void operands is void, 6.5.16p6, and its value
   is used as an operand of +. */
void f(void);
void g(void);

int
main(void)
{
        return 1 + (1 ? f() : g());
}
