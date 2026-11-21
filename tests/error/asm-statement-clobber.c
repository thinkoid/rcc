/* An __asm__ statement of an empty template whose clobber is a
   register's name, neither "memory" nor "cc", which is not yet. */
int
main(void)
{
        __asm__ __volatile__("" : : : "x19");

        return 0;
}
