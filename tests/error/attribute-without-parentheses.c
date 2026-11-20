/* An attribute specifier without its double parentheses. */
int f(void) __attribute__(nothrow);

int
main(void)
{
        return f();
}
