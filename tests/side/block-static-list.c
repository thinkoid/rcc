/* Two declarators of one static declaration, two objects. */
int
f(void)
{
        static int a = 1, b;
        return a + b;
}
