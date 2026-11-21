/* shape.md chapter 5, backend.md 7.2 and aarch64.md 7.2, a variable
   length array declared after a label of its function, whose
   declaration a goto back to the label reaches again.  The goto leaves
   the array's scope, so it puts the stack pointer back first, and the
   if around it is the condition's jump past the two, since its body is
   not one jump (if-jump).  The return leaves the storage to the
   epilogue, and the block's end, which no path reaches, writes
   nothing. */
int
f(int n)
{
        int k = 0;

again:
        k++;
        {
                char buf[n];

                buf[0] = 1;
                if (k < 2)
                        goto again;
                return buf[0];
        }
}

int
main(void)
{
        return f(1) != 1;
}
