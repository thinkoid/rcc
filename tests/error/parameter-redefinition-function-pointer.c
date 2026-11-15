/* Two parameters of the prototype of a pointer to a function with the
   same name: they share one scope, 6.7.1p4, though the function is not
   the one declared. */
int install(void (*h)(int a, int a));

int
main(void)
{
        return 0;
}
