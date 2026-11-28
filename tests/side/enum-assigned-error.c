/* An enumeration constant is no lvalue. */
enum e { A };

void
f(void)
{
        A = 1;
}
