/* A member designator, .x =, 6.7.11p1, which is not yet: designators
   are not yet, and no object of structure or union type, which alone
   takes one, p8, is defined yet. */
int table[2] = { .x = 1 };

int
main(void)
{
        return table[0];
}
