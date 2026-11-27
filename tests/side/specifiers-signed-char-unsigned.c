/* signed char unsigned: 'unsigned' cannot join the type specifiers
   before it, unsigned after signed char, an error at 'unsigned' in rcc's
   words, 6.7.3.1p2. */
int
f(void)
{
        signed char unsigned x;
        return 0;
}
