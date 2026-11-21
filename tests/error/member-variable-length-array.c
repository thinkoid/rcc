/* A member of a variable length array type: a member is no ordinary
   identifier, and only an ordinary identifier has a variably modified
   type, 6.7.7.3p2. */
void
f(int n)
{
        struct line
        {
                char buf[n];
        } l;
}

int
main(void)
{
        f(1);
        return 0;
}
