/* An array designator whose index is no integer constant expression,
   6.7.11p7. */
int n;
int table[3] = { [n] = 1 };

int
main(void)
{
        return table[0];
}
