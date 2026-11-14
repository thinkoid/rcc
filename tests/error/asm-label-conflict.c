/* Two declarations of one function under two assembler names: one
   name would link to two symbols. */
int f(void) __asm__ ("g");
int f(void) __asm__ ("h");

int main(void)
{
    return f();
}
