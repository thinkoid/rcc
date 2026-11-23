/* The qualifier _Atomic on a function type through a typedef, which
   6.7.4.1p4 forbids: the type it modifies is the typedef's,
   int (void). */
typedef int handler_t(void);
typedef _Atomic handler_t atomic_handler_t;

int
main(void)
{
        return 0;
}
