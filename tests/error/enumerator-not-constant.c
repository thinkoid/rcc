/* An enumeration constant's value read from an object: the value is an
   integer constant expression, 6.7.3.3p4, and an object's value is
   none, 6.6p8. */
int n = 3;

enum size { small = n, large };

int
main(void)
{
        return large;
}
