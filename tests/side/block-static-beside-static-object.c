/* A block's static beside a static object at file scope of its name,
   which another function reads. */
static int s = 1;
int
f(void)
{
        static int s = 2;
        return s;
}
int
g(void)
{
        return s;
}
