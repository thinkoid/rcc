/* A list of two expressions for a scalar, whose initializer is a
   single expression, optionally in braces, 6.7.11p12: the second is an
   initializer for an object not contained in the scalar, p3. */
int count = { 1, 2 };

int
main(void)
{
        return count;
}
