/* A block's extern of an object the file defines before it: the
   object's label. */
int g = 1;
int
f(void)
{
        extern int g;
        return g;
}
