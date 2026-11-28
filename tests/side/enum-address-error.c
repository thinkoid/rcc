/* The address of an enumeration constant, an rvalue. */
enum e { A };

int
f(void)
{
        return &A == 0;
}
