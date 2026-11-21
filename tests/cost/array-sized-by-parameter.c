/* shape.md chapter 5, a local array whose size is a parameter: a
   variable length array, though a parameter's array of the same size
   is the pointer the parameter is adjusted to.  The int converts to
   size_t as C converts it, sign-extended.  g calls nothing, but it
   takes storage from the stack, which would overlap a frame in the red
   zone, so it writes its frame's subq as a function that calls does;
   aarch64 has no red zone, and its frame is as any function's. */
int
g(int n)
{
        int a[n];

        return 0;
}

int
main(void)
{
        return g(3);
}
