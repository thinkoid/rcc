/* () declares no parameter, as (void) does, 6.7.7.4p13, so a definition
   of one parameter conflicts with it. */
int g();

int
g(int a)
{
        return a;
}
