/* register is the one storage class a parameter takes, 6.7.7.4p2:
   the error is at the extern. */
int f(extern int a);
