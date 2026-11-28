/* An array compared with an integer other than a null pointer constant
   is a pointer compared with one, an error at the operator. */
int
f(void)
{
        int a[3];
        return a == 1;
}
