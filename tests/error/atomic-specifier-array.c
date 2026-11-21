/* The atomic type specifier of an array type, which 6.7.3.5p3
   forbids. */
typedef _Atomic(int[3]) a_t;

int
main(void)
{
        return 0;
}
