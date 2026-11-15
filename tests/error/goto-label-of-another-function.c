/* A goto to a label of another function, 6.2.1p3: a label has function
   scope, so the label is not defined in main, an error. */
int
f(void)
{
out:
        return 1;
}

int
main(void)
{
        goto out;
}
