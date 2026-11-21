/* A bit-field of the name of a member of an anonymous struct declared
   before it: the anonymous member's members are the struct's,
   6.7.3.2p15, so a is a second member of one name, p1 and 6.2.3p1. */
struct flags
{
        struct
        {
                int a;
        };
        int a : 3;
};
