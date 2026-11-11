/* G13: x op= e evaluates x for its place, then e, then the operation
   as one piece: the read of x, the operator and the store. c1 writes
   the right operand's code first in each statement: the call to
   bump() before the add to a's slot and before the loads of b and c,
   d = 2 before the add to d's slot, r = 2 before addl %ebx, %ebx. So
   a, b and y are 11, c is 10, d and r are 4: exit status 63. The three
   statements with a call are not undefined in ISO C; d += (d = 2) and
   r += (r = 2) are (6.5.1p2), since the object is read and written
   unsequenced. clang -O2 writes main as movl $63, %eax: 63. */
int bump(int *p)
{
    *p = 10;
    return 1;
}

int main(void)
{
    int a = 1;
    int b = 1;
    int c = 1;
    int d = 1;
    int y;
    register int r = 1;

    a += bump(&a);
    y = b += bump(&b);
    c *= bump(&c);
    d += (d = 2);
    r += (r = 2);

    return (a == 11) + (b == 11) * 2 + (y == 11) * 4
           + (c == 10) * 8 + (d == 4) * 16 + (r == 4) * 32;
}
