/* A block's static initialized by an object at file scope: no constant
   expression, said at the initializer's first token. */
int g;
int
f(void)
{
        static int s = g;
        return s;
}
