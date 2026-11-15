/* A scalar's expression in two pairs of braces: the initializer for a
   scalar is a single expression, optionally enclosed in braces, one
   pair, 6.7.11p12. */
int count = { { 1 } };

int
main(void)
{
        return count;
}
