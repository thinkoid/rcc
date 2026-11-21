/* shape.md chapter 5, a local variable length array: an array whose
   size is a local's value, its storage taken at run time where the
   declaration is reached.  Neither generator writes the stack
   allocation yet, so the program has no pin on either target. */
int
main(void)
{
        int n = 3;
        int a[n];

        return 0;
}
