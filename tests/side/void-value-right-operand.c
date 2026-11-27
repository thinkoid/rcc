/* A void call's value as the right operand of +: the error is at the
   operator, before the left operand's type is judged. */
void g(void);
int h(void);

int
f(void)
{
        return h + g();
}
