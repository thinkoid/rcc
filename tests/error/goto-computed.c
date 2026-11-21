/* GNU C's computed goto, which is not yet; the address of a label
   after it makes the program GNU C's. */
int
main(void)
{
        void *p = 0;

        if (p)
                goto *p;

        p = &&next;
next:
        return 0;
}
