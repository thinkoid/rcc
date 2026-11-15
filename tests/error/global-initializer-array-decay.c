/* An array as a pointer's initializer, which decays to an address
   constant, 6.6p11, not yet. */
int table[3];
int *pointer = table;

int
main(void)
{
        return *pointer;
}
