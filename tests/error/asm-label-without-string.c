/* An assembler name that is an identifier, not a string literal. */
int f(void) __asm__ (g);

int main(void)
{
    return f();
}
