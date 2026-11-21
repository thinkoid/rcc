/* An __asm__ statement of an empty template with an input operand,
   which is not yet. */
int
main(void)
{
        int x = 1;

        __asm__ __volatile__("" : : "r"(x));

        return x;
}
