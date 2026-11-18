/* A tentative definition of internal linkage of an object of an
   incomplete struct, 6.9.3p3. */
struct point;

static struct point origin;

int
main(void)
{
        return 0;
}
