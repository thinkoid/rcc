/* A character outside ASCII in a character constant of no prefix: its
   UTF-8 is two bytes, and a char holds one, 6.4.4.5p11. */
int
main(void)
{
        return 'é';
}
