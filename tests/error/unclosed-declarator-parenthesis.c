/* A declarator in parentheses without the parenthesis that closes it:
   the declaration ends first, and the error is at its semicolon. */
int
main(void)
{
    int (*p[3];

    return 0;
}
