/* Chapter 3, a local: one stack slot, a store at each write and a
   load at each read.  The listing must show x stored to its own slot
   at the initializer and at the assignment, and loaded from it at
   each of the two reads; x stays in memory between the statements. */
int local(int a)
{
    int x = a;
    x = x + 1;
    return x;
}
