/* Two string literals in braces for an array of char: a string literal
   in braces initializes the array whole, 6.7.11p15, and the second is
   an initializer for an object not contained in the array, p3. */
char words[] = { "one", "two" };

int
main(void)
{
        return words[0];
}
