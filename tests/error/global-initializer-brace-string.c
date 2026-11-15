/* Two string literals in braces, an initializer list of an array of
   char: a list is not yet, and only one string literal in braces is
   taken, as the literal alone is, 6.7.11p15. */
char words[] = { "one", "two" };

int
main(void)
{
        return words[0];
}
