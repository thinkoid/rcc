/* signed signed: 'signed' cannot join the type specifiers before it,
   a second signed, an error at 'signed' in rcc's words, 6.7.3.1p2. */
int
f(void)
{
        signed signed x;
        return 0;
}
