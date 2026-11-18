/* _Alignof of an expression, which GNU C takes and C does not,
   6.5.4.4p1: not yet. */
int x;

int
main(void)
{
        return _Alignof x;
}
