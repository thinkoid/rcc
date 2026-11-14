/* A function defined with a union result: the declaration is accepted,
   and the definition, which makes the object it returns, is not yet. */
union word { int i; char c; };

union word zero(void);

union word zero(void)
{
    union word w;

    w.i = 0;
    return w;
}

int main(void)
{
    return 0;
}
