/* A block's static object: no linkage, its label the function's name
   and its own, f.s, in .bss without .globl, and its declaration no
   statement. */
int
f(void)
{
        static int s;
        return s;
}
