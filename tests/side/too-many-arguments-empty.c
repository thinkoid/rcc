/* An argument to a function declared with (), which declares no
   parameter, 6.7.7.4p13. */
int g();

int
f(void)
{
        return g(1);
}
