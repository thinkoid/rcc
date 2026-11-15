/* A designator in an initializer list, 6.7.11p1, which is not yet. */
int table[4] = { 1, [2] = 5 };

int
main(void)
{
        return table[0];
}
