/* A tentative definition of internal linkage of an object of a struct
   that the unit never completes: the object takes the struct as it is
   at the end of the unit, 6.9.3p2, which is still incomplete, 6.9.3p3,
   an error then. */
struct point;

static struct point origin;

int
main(void)
{
        return 0;
}
