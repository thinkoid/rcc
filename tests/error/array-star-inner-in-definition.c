/* '*' for the size of an array behind the pointer a parameter of a
   definition is: a variable length array of unspecified size is no
   part of a parameter declaration in a function definition, 6.9.2p6,
   in any of its brackets. */
int
f(int (*a)[*])
{
        return a[0][0];
}

int
main(void)
{
        return 0;
}
