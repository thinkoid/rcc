/* Two parameters of one name in a definition: the parameters of a list
   share its scope, 6.7.1p4, and the error is at the second's name. */
int
g(int a, int a)
{
        return a;
}
