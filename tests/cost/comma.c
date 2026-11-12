/* backend.md 6.6 and aarch64.md 6.6, a, b: the left operand of a comma
   is evaluated for its effect, wherever the comma stands, and the right
   one in the comma's own context.

   On x86-64 the listing of sum() must show the initializer's two
   constants each stored in one instruction, movl $0, -8(%rbp) and
   movl $0, -12(%rbp), and the step as addl $1, -8(%rbp) and
   addl $2, -12(%rbp), with no load, no copy of an old value and no
   store; of last(), a++ as addl $1, -4(%rbp) and then b loaded for the
   value; and of test(), a = b as b loaded and stored to a's slot, then
   c loaded and compared for the jump.

   On aarch64 the listing of sum() must show the initializer's two
   constants, both zero, each stored in one instruction,
   str wzr, [x29, #20] and str wzr, [x29, #24], and the step as a load,
   an add and a store for each, add w0, w0, #1 and add w0, w0, #2, since
   the machine has no memory form, with no copy of an old value; of
   last(), a++ as the load, add w0, w0, #1 and the store, and then b
   loaded for the value; and of test(), a = b as b loaded and stored to
   a's slot, then c loaded and tested with cbz w0 for the jump. */
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
