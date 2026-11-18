/* A conditional of one void operand and one of a value: the two meet
   in none of the pairs 6.5.16p3 allows. */
void f(void);
int g(void);

int
main(void)
{
        1 ? f() : g();

        return 0;
}
