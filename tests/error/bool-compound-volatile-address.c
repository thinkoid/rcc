/* A compound assignment to a _Bool whose address reads a volatile
   object: rcc builds b op= e as b = (_Bool) (b op (e)), 6.5.17.3p4,
   which names b twice, and the read of the volatile pointer p is a
   side effect, 5.1.2.4p2, which the source writes once, so *p |= x on
   a _Bool *volatile p is not yet. */
_Bool *volatile p;

void
mark(int x)
{
        *p |= x;
}

int
main(void)
{
        return 0;
}
