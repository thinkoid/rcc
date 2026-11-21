/* GNU C's array designator without its =, [1] 2, which is not
   yet. */
int table[3] = { [1] 2 };

int
main(void)
{
        return table[1];
}
