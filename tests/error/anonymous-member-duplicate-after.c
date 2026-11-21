/* A struct that declares a member of a name its anonymous struct, in
   an anonymous union, already has: the anonymous members' members are
   the struct's at any depth, 6.7.3.2p15, so y is a second member of
   one name, p1 and 6.2.3p1. */
struct value
{
        union
        {
                struct
                {
                        int y;
                };
                long l;
        };
        int y;
};
