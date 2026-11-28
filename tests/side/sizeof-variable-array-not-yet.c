/* A variable length array in a type name is rcc's not yet. */
unsigned long
f(int n)
{
        return sizeof(int[n]);
}
