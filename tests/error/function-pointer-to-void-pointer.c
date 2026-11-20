/* A pointer to a function assigned to a void *: assignment converts
   a pointer to void only to and from a pointer to an object type,
   6.5.17.2p1, and a function is not an object, so ISO C asks a cast.
   GNU C converts it as the cast does, and rcc so, with a warning, and
   the program compiles. */
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
