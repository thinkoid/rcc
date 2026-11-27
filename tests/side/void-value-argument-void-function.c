/* A void call's value as an argument of a call in a void function's
   body. */
void g(void);
void h(int a);

void
f(void)
{
        h(g());
}
