/* A cast's type name is read by the declaration specifiers too, so
   (long char) draws the error of a local's, at 'char'. */
int
f(void)
{
        int x = 0;
        return (long char) x;
}
