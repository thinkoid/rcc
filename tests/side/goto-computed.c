/* GNU C's computed goto, goto *p;, which rcc does not compile, refused
   at the keyword in rcc's words. */
int
main(void)
{
        int a = 1;
        goto *a;
        return 0;
}
