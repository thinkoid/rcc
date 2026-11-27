/* A function declared without a storage class, then defined static,
   6.2.2p7: the error is at the definition's name. */
int g(void);

static int
g(void)
{
        return 1;
}
