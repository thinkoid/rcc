/* sizeof applied to a function, which has no size. */
int f(void);

int
main(void)
{
        return sizeof f;
}
