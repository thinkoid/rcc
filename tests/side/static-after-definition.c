/* A function defined without a storage class, then declared static,
   6.2.2p7. */
int
g(void)
{
        return 1;
}

static int g(void);
