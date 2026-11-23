/* The qualifier _Atomic on an array type through a typedef, which
   6.7.4.1p4 forbids: the type it modifies is the typedef's, int [3]. */
typedef int triple_t[3];
typedef _Atomic triple_t atomic_triple_t;

int
main(void)
{
        return 0;
}
