/* backend.md 6.6, a, b: the left operand of a comma is evaluated for
   its effect, wherever the comma stands, and the right one in the
   comma's own context.  The listing of sum() must show the
   initializer's two constants each stored in one instruction,
   movl $0, -8(%rbp) and movl $0, -12(%rbp), and the step as
   addl $1, -8(%rbp) and addl $2, -12(%rbp), with no load, no copy of
   an old value and no store; of last(), a++ as addl $1, -4(%rbp) and
   then b loaded for the value; and of test(), a = b as b loaded and
   stored to a's slot, then c loaded and compared for the jump. */
int sum(int n)
{
    int i;
    int j;
    int s = 0;

    for (i = 0, j = 0; i < n; i++, j += 2)
        s += j;

    return s;
}

int last(int a, int b)
{
    return (a++, b);
}

int test(int a, int b, int c)
{
    if (a = b, c)
        return a;

    return 0;
}
