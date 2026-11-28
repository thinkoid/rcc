/* Two anonymous members' members are the record's, 6.7.3.2p15. */
struct s
{
        struct
        {
                int a;
        };
        struct
        {
                long a;
        };
};
