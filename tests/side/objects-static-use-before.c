/* A static object used between its tentative definition and its
   definition with an initializer. */
static int x;
int
f(void)
{
        return x;
}
static int x = 6;
