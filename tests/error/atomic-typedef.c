/* A typedef through the qualifier _Atomic: the typedef is accepted
   and names a type rcc does not use yet, and the object a pointer to
   it points to is not yet. */
typedef _Atomic int counter_t;

counter_t *counter(void);

int
main(void)
{
        return *counter();
}
