/* '*' for the size of an array no parameter declaration holds: a
   variable length array of unspecified size stands only in the
   declarators of a parameter declaration, 6.7.7.3p4. */
int x[*];

int
main(void)
{
        return 0;
}
