/* A label's name is defined once in a function, 6.8.2p3: the second
   definition is an error at its name. */
int
main(void)
{
        int a = 1;
L:
        a = 1;
L:
        return a;
}
