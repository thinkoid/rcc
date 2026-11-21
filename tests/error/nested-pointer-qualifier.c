/* A pointer to a pointer to char converted to a pointer to a pointer
   to const char: the pointees, char * and const char *, are qualified
   apart and so not compatible, 6.7.4.1p11, and the assignment would
   let a const char be written through the char *, 6.5.17.2's EXAMPLE
   3. */
int
main(void)
{
        char c = 'a';
        char *p = &c;
        char **pp = &p;
        const char **cpp = pp;

        return 0;
}
