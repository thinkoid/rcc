/* GNU C's designator of a range of elements, [0 ... 2] =, which is
   not yet. */
int table[3] = { [0 ... 2] = 1 };

int
main(void)
{
        return table[0];
}
