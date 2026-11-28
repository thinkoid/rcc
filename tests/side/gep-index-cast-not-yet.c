/* An index of an object under a cast to its width narrower than an int,
   whose widening the builder reads as the object at the cast's type
   where aarch64's index form reads the cast, is not yet, at the cast. */
int
f(int *p, char c)
{
        return p[(unsigned char) c];
}
