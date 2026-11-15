/* A typedef through volatile, as bits/pthreadtypes.h declares
   pthread_spinlock_t: the typedef is accepted and names a type rcc
   does not use yet, a pointer to it is an ordinary pointer, and a
   local of it is not yet. */
typedef volatile int spinlock_t;

int spin_lock(spinlock_t *lock);

int
main(void)
{
        spinlock_t lock;

        return 0;
}
