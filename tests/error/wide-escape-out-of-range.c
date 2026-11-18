/* A hexadecimal escape past the 16 bits of a char16_t in a UTF-16
   character constant, 6.4.4.5p9. */
int
main(void)
{
        return u'\x10000';
}
