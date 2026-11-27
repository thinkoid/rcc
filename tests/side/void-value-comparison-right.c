/* A void call's value as the right operand of a comparison whose left
   operand is a function's name: both values are read before their
   types are judged, so the void one is the error. */
void g(void);
int h(void);

int
f(void)
{
        return h < g();
}
