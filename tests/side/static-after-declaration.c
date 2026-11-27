/* A function declared without a storage class, then static: the name
   would have external and internal linkage in one unit, 6.2.2p7, and
   the error is at the later declaration's name. */
int g(void);
static int g(void);
