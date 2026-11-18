/* A wide character constant of two characters, 6.4.4.5p10 for u and
   U; clang refuses it after L too. */
int
main(void)
{
        return L'ab';
}
