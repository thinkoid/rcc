/* An object used before its definition later in the unit: the unit is
   parsed whole before it is generated, so the read is the label's,
   movl x(%rip), %eax. */
extern int x;
int
f(void)
{
        return x;
}
int x;
