/* A label where a statement is wanted, an if's then part, labels the
   statement after it, 6.8.2p1, so a closing brace there is an
   expression statement's missing expression. */
int
main(void)
{
        int a = 1;
        if (a)
        L:
}
