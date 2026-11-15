/* The initializer of an object of static storage is a constant
   expression, 6.7.11p5; the value of another object is none. */
int base;
int count = base + 1;

int
main(void)
{
        return count;
}
