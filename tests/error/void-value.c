/* The result of a void function used as a value. */
void g(void);

int
main(void)
{
        int x = g();

        return x;
}
