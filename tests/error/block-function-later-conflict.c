/* A function declared in a block and defined after it at file scope
   with another type: every declaration of a name of external linkage
   denotes one function, 6.2.2p2, of one type, 6.2.7p2. */
int
main(void)
{
        int twice(int x);

        return twice(2);
}

long
twice(int x)
{
        return x + x;
}
