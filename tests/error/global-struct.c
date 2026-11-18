/* A tentative definition at file scope of an object of a struct that
   the unit never completes: the object takes the struct as it is at
   the end of the unit, 6.9.3p2, which has no size, an error. */
struct point;
struct point origin;
int
main(void)
{
        return 0;
}
