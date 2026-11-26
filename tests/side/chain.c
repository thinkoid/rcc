/* A chain of 9 operators nested on the right, each level's left
   operand held while its right one is generated.  The innermost a + a
   holds one register on x86-64, its right a the operand in memory, and
   each level one more: 9 levels fill the nine expression registers,
   eax to r11d, and spill nothing.  aarch64 holds the innermost right a
   in a register too, so 10 of its fifteen. */
int
main(void)
{
        int a = 1;
        return a + (a - (a & (a | (a ^ (a + (a - (a & (a | a))))))));
}
