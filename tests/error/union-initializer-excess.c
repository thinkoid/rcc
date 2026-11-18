/* A union's list with a second initializer, where the list gives a
   value to the union's first member alone, 6.7.11p18, p3: an error. */
union number
{
        int i;
        long l;
};

union number one = { 1, 2 };

int
main(void)
{
        return one.i;
}
