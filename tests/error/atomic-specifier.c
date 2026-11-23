/* The atomic type specifier, as stdatomic.h declares atomic_int,
   dropped with a warning: the typedef names int, and a local of it is
   an int. */
typedef _Atomic(int) atomic_int;

int
main(void)
{
        atomic_int n;

        return 0;
}
