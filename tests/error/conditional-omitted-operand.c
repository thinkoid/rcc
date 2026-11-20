/* GNU C's a ?: b, the conditional whose middle operand is left out,
   which is a evaluated once: rcc does not compile it yet. */
int
main(void)
{
        int a = 0;

        return a ?: 1;
}
