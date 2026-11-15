/* The atomic type specifier, as stdatomic.h declares atomic_int, which
   is not yet. */
typedef _Atomic(int) atomic_int;

int
main(void)
{
        return 0;
}
