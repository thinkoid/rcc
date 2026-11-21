/* GNU C's a ?: b of a void a: the condition shall be a scalar,
   6.5.16p2, and a void expression is no value. */
void f(void);

int
main(void)
{
        return f() ?: 1;
}
