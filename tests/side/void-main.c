/* main defined with the result type void, which 5.1.2.3.2p1 admits as
   an implementation's manner and rcc takes with no diagnostic: falling
   off its end writes the return of 0 all the same, since the rule is
   main's name's, 5.1.2.3.4p1. */
void
main(void)
{
}
