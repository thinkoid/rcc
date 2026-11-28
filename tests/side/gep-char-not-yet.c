/* Arithmetic on a pointer to a type other than int or a pointer is not
   yet, at its operator. */
int
f(void)
{
        char c;
        return &c + 1 == 0;
}
