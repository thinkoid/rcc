/* A designator in the braces of a scalar, which has no subobject to
   name, 6.7.11p7, p12. */
int count = { [0] = 1 };

int
main(void)
{
        return count;
}
