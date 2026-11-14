/* A call of a function whose result is long double: the declaration is
   accepted, and the call, which makes the object it returns, is not
   yet, though its value is dropped. */
long double half(int);

int main(void)
{
    half(1);
    return 0;
}
