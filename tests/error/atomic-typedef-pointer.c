/* A pointer to int passed for a pointer to a typedef through _Atomic:
   an atomic type is compatible with no type but itself, 6.7.4.1p11,
   so the two pointers are incompatible, 6.5.17.2p1, and the
   conversion is the error it is, not a refusal of what rcc does not
   know. */
typedef _Atomic int counter_t;

void f(counter_t *c);

void
g(int *p)
{
        f(p);
}

int
main(void)
{
        return 0;
}
