/* A local of the name of a parameter of another type, in the body's
   own block: the error is at the local's name. */
int
g(short a)
{
        long a;
        return 0;
}
