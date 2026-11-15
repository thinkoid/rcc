/* An array at file scope sized by an object, and a typedef of one:
   an identifier of a variably modified type has block or function
   prototype scope, and an object of static storage duration has no
   variable length array type, 6.7.7.3p2.  The first is the error. */
extern int count;
int table[count];
typedef int row_t[count];

int
main(void)
{
        return 0;
}
