/* A call with more arguments than the prototype has parameters,
   6.5.3.3p2. */
int f(int a);

int main(void)
{
    return f(1, 2);
}
