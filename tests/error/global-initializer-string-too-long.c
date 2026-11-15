/* A string literal longer than the array of char it initializes:
   6.7.11p15 takes the terminating zero only when there is room, but
   every other byte is an element, and p3 refuses an initializer for an
   element the array does not have. */
char greeting[4] = "hello";

int
main(void)
{
        return greeting[0];
}
