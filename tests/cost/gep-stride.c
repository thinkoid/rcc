/* Chapter 3, p + n; chapter 4, a stride that is not a power of two.
   m[i] on int m[2][3] is a row of 12 bytes, taken as a pointer: the
   listing must show the index widened with cltq, scaled by
   imul $0xc and added to the array's address from lea. */
int *row(int i)
{
    int m[2][3];
    int *p;

    p = m[i];
    return p;
}
