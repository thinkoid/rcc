/* The type qualifier _Atomic among the declaration specifiers, dropped
   with a warning: n is an int, and the program compiles. */
int
main(void)
{
        _Atomic int n = 0;

        return n;
}
