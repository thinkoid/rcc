/* An assembler name without its closing parenthesis. */
int f(void) __asm__ ("g";

int main(void)
{
    return f();
}
