/* register on the void of an empty parameter list, which J.2 (75)
   leaves undefined: the special case is an unnamed parameter of type
   void, 6.7.7.4p9. */
int f(register void);

int
main(void)
{
        return 0;
}
