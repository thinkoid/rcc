/* A parameter named as a typedef: the parameter is an object in the
   scope of the parameter list from the end of its declarator, 6.2.1p7,
   so it hides the typedef name there, and the second T names the
   object, no type. */
typedef int T;

int f(int T, T x);

int
main(void)
{
        return 0;
}
