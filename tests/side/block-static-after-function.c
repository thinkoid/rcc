/* A block's static in the second of two functions, and an object at
   file scope after them: the data in the order of the entries. */
int
g(void)
{
        return 0;
}
int
f(void)
{
        static int s = 3;
        return s;
}
int h;
