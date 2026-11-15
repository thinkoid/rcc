/* The atomic type specifier, as stdatomic.h declares atomic_int: the
   typedef is accepted and names a type rcc does not use yet, and a
   local of it is not yet. */
typedef _Atomic(int) atomic_int;

int
main(void)
{
        atomic_int n;

        return 0;
}
