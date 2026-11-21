/* A variable length array with an initializer list: such an entity is
   initialized by an empty initializer alone, 6.7.11p4. */
void
f(int n)
{
        char buf[n] = { 0 };
}

int
main(void)
{
        f(1);
        return 0;
}
