/* A call with fewer arguments than the prototype has parameters,
   6.5.3.3p2. */
int f(int a, int b);

int main(void)
{
    return f(1);
}
