/* shape.md chapter 5, backend.md chapter 5 and aarch64.md 5.2, a
   local variable length array: an array whose size is a local's value,
   its storage taken at run time where the declaration is reached.  The
   size, n converted to size_t and times the element's 4, goes to a
   hidden local, and the stack pointer is kept in another; then it
   moves down by the size and down again to a multiple of 16, and the
   array's pointer is the stack pointer after the move, formed on
   aarch64 in x0 below sp, where it is rounded down before sp moves to
   it.  The array's scope is the function's body, whose end is the
   epilogue, so no restore is written; on aarch64 the epilogue moves
   sp back to x29 first. */
int
main(void)
{
        int n = 3;
        int a[n];

        return 0;
}
