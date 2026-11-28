/* The operand of -> points to a record, 6.5.3.4p2. */
int
f(int *p)
{
        return p->a;
}
