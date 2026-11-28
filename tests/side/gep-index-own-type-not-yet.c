/* An index of an object under a cast to its own type, which the builder
   reads as the object where aarch64's index form reads the cast and
   extends it again, is not yet, at the cast. */
int
f(int *p, unsigned char c)
{
        return p[(unsigned char) c];
}
