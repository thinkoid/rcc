/* An __asm__ statement with volatile written twice: GNU C takes each
   of its qualifiers, volatile, inline and goto, once, so the second
   volatile is an error, though the statement is the barrier. */
int
main(void)
{
        __asm__ volatile volatile("" : : : "memory");

        return 0;
}
