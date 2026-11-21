/* The atomic type specifier of a function type, which 6.7.3.5p3
   forbids. */
typedef _Atomic(int(void)) f_t;

int
main(void)
{
        return 0;
}
