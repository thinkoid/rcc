/* Two parameters of one prototype with the same name: they share one
   scope, 6.7.1p4. */
int f(int a, int a);

int
main(void)
{
        return 0;
}
