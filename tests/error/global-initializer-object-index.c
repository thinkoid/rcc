/* The address of an element whose index is an object: an address
   constant reads the value of no object, 6.6p12, and the initializer of
   an object of static storage is a constant expression, 6.7.11p5. */
int table[4];
int slot;
int *pointer = &table[slot];

int
main(void)
{
        return *pointer;
}
