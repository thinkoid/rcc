/* Two functions' names added: two pointers are no sum, 6.5.7p2. */
int g(void);

int
f(void)
{
        return g + g;
}
