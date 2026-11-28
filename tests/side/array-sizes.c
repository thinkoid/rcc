/* sizeof of an array is the array's, which its operand does not decay
   to, 6.3.2.1p3; of its decay, a pointer's; and of a type name, an
   array's. */
unsigned long
sizes(int i)
{
        int a[4];
        int *b[3];
        int c[sizeof(int) * 2];
        int d[(int) 3];
        return sizeof a + sizeof(a) + sizeof a[0] + sizeof *a + sizeof(a + 0) +
               sizeof b + sizeof c + sizeof d + sizeof(int[5]) +
               sizeof(int *[5]) + sizeof a / sizeof a[0] + sizeof &a;
}
