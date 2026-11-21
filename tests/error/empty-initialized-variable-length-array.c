/* A variable length array with the empty initializer, which C23 takes,
   6.7.11p4, and which would zero an object whose size is known at run
   time alone: rcc does not compile it yet. */
void
f(int n)
{
        char buf[n] = { };
}

int
main(void)
{
        f(1);
        return 0;
}
