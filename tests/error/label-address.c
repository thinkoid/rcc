/* GNU C's address of a label, which is not yet. */
int
main(void)
{
        void *p = &&next;

next:
        return p != 0;
}
