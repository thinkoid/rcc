/* A typedef at file scope of an array sized by an object: a typedef
   name of a variably modified type has block or function prototype
   scope, 6.7.7.3p2. */
extern int count;
typedef int row_t[count];

int
main(void)
{
        return 0;
}
