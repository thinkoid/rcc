/* An assembler name without its parentheses. */
int f(void) __asm__ "g";

int main(void)
{
    return f();
}
