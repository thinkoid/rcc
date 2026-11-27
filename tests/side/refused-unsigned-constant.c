/* A constant of an unsigned type is not yet compiled: 0x80000000 is an
   unsigned, the first type of its list that holds it, 6.4.4.2p6. */
long
f(void)
{
        return 0x80000000;
}
