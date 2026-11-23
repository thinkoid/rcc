/* A pointer to int passed for a pointer to a typedef through _Atomic:
   an atomic type is compatible with no type but itself, 6.7.4.1p11,
   but rcc drops the qualifier with a warning at the typedef, which
   names int, so the two pointers are one type and the call
   compiles. */
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
