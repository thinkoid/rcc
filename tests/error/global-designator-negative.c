/* An array designator of a negative index, which names no element
   of the array, 6.7.11p3, p7. */
int table[3] = { [-1] = 1 };

int
main(void)
{
        return table[0];
}
