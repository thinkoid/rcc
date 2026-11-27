/* An object at file scope read in a function: a tentative definition,
   so its label, movl x(%rip), %eax. */
int x;

int
f(void)
{
        return x;
}
