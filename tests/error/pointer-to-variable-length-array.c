/* A pointer to a variable length array, whose type is variably
   modified, 6.7.7.1p3: rcc does not compile it yet. */
int
f(int n, char *p)
{
        char (*row)[n] = (void *) p;

        return row[0][0];
}

int
main(void)
{
        char c = 0;

        return f(1, &c);
}
