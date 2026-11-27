/* A function's name added to: its pointer points to no object, 6.5.7p2,
   an error at the '+'. */
int g(void);

int
f(void)
{
        return 1 + g;
}
