/* A typedef through the qualifier _Atomic, dropped with a warning: the
   typedef names int, and the object a pointer to it points to is read
   with the plain load. */
typedef _Atomic int counter_t;

counter_t *counter(void);

int
main(void)
{
        return *counter();
}
