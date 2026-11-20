/* A function type as the type name of sizeof: a function has no size,
   6.5.4.4p1. */
int
main(void)
{
        return sizeof(int(void));
}
