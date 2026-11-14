/* A parameter of type float in a function definition: the declaration
   is accepted, and the parameter, an object of the definition, is not
   yet. */
int truncated(float x);

int truncated(float x)
{
    return (int) x;
}

int main(void)
{
    return 0;
}
