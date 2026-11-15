/* An array whose elements are functions, the element type a typedef
   name's, 6.7.7.3p1. */
typedef int handler(int);

int
main(void)
{
        handler table[2];

        return 0;
}
