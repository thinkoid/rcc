/* A block's static of a parameter's name in the body's outermost block,
   which is the parameters' scope. */
int
f(int a)
{
        static int a;
        return a;
}
