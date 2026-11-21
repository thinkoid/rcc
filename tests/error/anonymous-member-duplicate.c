/* A struct whose anonymous union declares a member of a name the
   struct already has: the anonymous member's members are the struct's,
   6.7.3.2p15, so x is a second member of one name, p1 and 6.2.3p1,
   refused at the ';' that ends the anonymous member, since its list
   was read before the member was known to be anonymous. */
struct value
{
        int x;
        union
        {
                int x;
                long l;
        };
};
