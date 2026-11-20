/* An attribute list whose attribute is no name. */
int f(void) __attribute__((42));

int
main(void)
{
        return f();
}
