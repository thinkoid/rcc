/* A local array of char initialized by a string literal in braces,
   6.7.11p15, which is not yet, as without them. */
int
main(void)
{
        char s[4] = { "abc" };

        return s[0];
}
