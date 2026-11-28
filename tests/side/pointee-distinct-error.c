/* Two pointers to integer types of one width are distinct pointer
   types to a comparison, an error at the operator. */
int
f(long *p, unsigned long *q)
{
        return p >= q;
}
