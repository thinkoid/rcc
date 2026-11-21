/* An array designator past the array's last element: an initializer
   for an object not contained in the array, 6.7.11p3. */
int table[3] = { [3] = 1 };

int
main(void)
{
        return table[0];
}
