/* A local array initialized by an expression that is no string
   literal: an array takes a brace-enclosed list, 6.7.11p17. */
int
main(void)
{
        int a[3] = 5;

        return a[0];
}
