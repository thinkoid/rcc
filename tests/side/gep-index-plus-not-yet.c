/* An index of an object under two unary pluses, which keep its bits as
   a cast to its width does, is not yet, at the first. */
unsigned u;

int
f(int *p)
{
        return *(p + +(+u));
}
