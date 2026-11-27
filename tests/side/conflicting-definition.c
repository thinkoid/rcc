/* A definition whose type is not that of the declaration before it,
   6.7.1p5: the error is at the definition's name. */
void g(void);

int
g(void)
{
        return 1;
}
