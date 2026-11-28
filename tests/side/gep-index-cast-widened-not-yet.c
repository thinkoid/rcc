/* An index of a widening of an object under a cast to its width, a
   change of sign among them, is not yet, at the cast. */
int
f(int *p, signed char c)
{
        return p[(int) (unsigned char) c];
}
