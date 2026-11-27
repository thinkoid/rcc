/* Two functions' externs of one name are one entity. */
int
f(void)
{
        extern int e;
        return e;
}
int
h(void)
{
        extern int e;
        return e + 1;
}
