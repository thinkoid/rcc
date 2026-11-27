/* void signed: 'signed' cannot join the type specifiers before it,
   signed after void, an error at 'signed' in rcc's words, 6.7.3.1p2. */
int
f(void)
{
        void signed x;
        return 0;
}
