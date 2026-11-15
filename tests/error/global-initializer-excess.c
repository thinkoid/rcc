/* More initializers than the array has elements: an initializer for
   an object not contained in the array, 6.7.11p3. */
int table[2] = { 1, 2, 3 };

int
main(void)
{
        return table[0];
}
