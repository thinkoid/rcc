/* A string literal as a char array's initializer, 6.7.11p15, which is
   not yet. */
char greeting[6] = "hello";

int
main(void)
{
        return greeting[0];
}
