/* An argument of a parameter of a type other than int is not yet
   compiled, refused at its first token. */
int g(char a);

int
f(void)
{
        return g(1);
}
