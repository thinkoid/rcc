/* A block's static object declared twice in its block: a name of no
   linkage is declared once in a scope, 6.7.1p4. */
int
main(void)
{
        static int count;
        static int count;

        return count;
}
