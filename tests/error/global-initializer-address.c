/* The address of an object as an initializer, an address constant,
   6.6p11, which is not yet. */
int count;
int *pointer = &count;

int
main(void)
{
        return *pointer;
}
