/* An object declared static, then without a storage class: the first
   declaration gives the name internal linkage, 6.2.2p3, the second
   external, p5, and a name of both linkages in one unit is undefined,
   p7. */
static int count;
int count;

int
main(void)
{
        return count;
}
