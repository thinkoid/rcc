/* An element of a list that is no constant expression, the value of
   another object: every expression in the initializer of an object of
   static storage is a constant expression, 6.7.11p5. */
int base;
int table[2] = { 1, base };

int
main(void)
{
        return table[0];
}
