/* A pointer to a function assigned to a void *: assignment converts
   a pointer to void only to and from a pointer to an object type,
   6.5.17.2p1, and a function is not an object.  A cast converts it. */
void *
erase(void (*h)(int))
{
        void *v = (void *) h;

        v = h;
        return v;
}

int
main(void)
{
        return 0;
}
