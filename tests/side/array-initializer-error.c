/* An array is initialized by a list in braces, or a string, 6.7.11p17:
   any other initializer is an error at its first token. */
int
f(void)
{
        int a[3] = 5;
        return 0;
}
