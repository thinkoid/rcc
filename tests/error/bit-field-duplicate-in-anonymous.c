/* An anonymous struct whose bit-field has the name of a member declared
   before it: the anonymous member's members are the struct's,
   6.7.3.2p15, its bit-fields among them, so a is a second member of one
   name, p1 and 6.2.3p1. */
struct flags
{
        int a;
        struct
        {
                int a : 1;
        };
};
