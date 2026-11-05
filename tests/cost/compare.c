/* Chapter 3, a < b and the rest: cmp, setcc, and $1.  The listing
   must show one cmp and one setl in less(), one cmp and one sete in
   equal(), each widened to int, and no conditional jump. */
int less(int a, int b)
{
    return a < b;
}

int equal(int a, int b)
{
    return a == b;
}
