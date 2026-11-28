/* A typedef name declared again in a block names the same type: char
   and signed char are two, 6.2.5p20. */
int
f(void)
{
        typedef char T;
        typedef signed char T;
        return 0;
}
