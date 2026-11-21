/* A local array of char initialized by a string literal, 6.7.11p15,
   whose bytes a local would take at run time, which is not yet. */
int
main(void)
{
        char s[] = "abc";

        return 0;
}
