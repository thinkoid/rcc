/* An anonymous member's members are the record's, 6.7.3.2p15, so their
   names are distinct from its own. */
struct s
{
        int a;
        struct
        {
                int a;
        };
};
