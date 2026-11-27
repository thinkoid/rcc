/* A void call's value as a for's condition; a void call as the init or
   the step is evaluated for its effect alone, and is no error. */
void g(void);

int
f(void)
{
        for (g();; g())
                break;
        for (; g();)
                ;
        return 0;
}
