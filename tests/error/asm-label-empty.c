/* An assembler name of no characters, which names no symbol: the
   adjacent literals are joined first, as glibc writes them. */
int f(void) __asm__ ("" "");

int main(void)
{
    return f();
}
