/* sizeof of a function designator is an error at the sizeof. */
int g(void);

int
f(void)
{
        return sizeof g;
}
