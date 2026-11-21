/* A member designator for an array, which is no struct or union,
   6.7.11p8. */
int table[3] = { .x = 1 };

int
main(void)
{
        return table[0];
}
