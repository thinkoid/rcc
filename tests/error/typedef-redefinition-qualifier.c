/* A typedef declared again with a qualifier it did not have: a
   qualified version of a type is a type of its own, 6.2.5p31, and a
   typedef name declared again denotes the same type, 6.7.1p4. */
typedef int count;
typedef const int count;

int
main(void)
{
        return 0;
}
