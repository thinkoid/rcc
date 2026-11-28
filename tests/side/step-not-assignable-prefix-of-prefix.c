/* ++++a is ++(++a), and a step's value is no lvalue, so the outer ++,
   the first, has no modifiable lvalue. */
int g(int x);

int
main(void)
{
        int a = 1;
        int b = 2;
        ++ ++a;
        return a;
}
