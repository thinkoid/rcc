/* static with '*' for the size in a parameter's brackets: static takes
   a size, 6.7.7.3p3, and '*' is none. */
int f(int a[static * ]);

int
main(void)
{
        return 0;
}
