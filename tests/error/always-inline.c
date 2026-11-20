/* A function declared always_inline: rcc inlines no call, so the
   attribute is dropped with a warning that says so, the call is
   written, and the program compiles. */
static inline int __attribute__((always_inline))
step(int n)
{
        return n + 1;
}

int
main(void)
{
        return step(-1);
}
