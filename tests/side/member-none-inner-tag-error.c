/* A tag declared in a member list declares no member, 6.7.3.2p2, so its
   members are none of the record's. */
struct s
{
        int k;
        struct t
        {
                int z;
        };
};

int
f(struct s *p)
{
        return p->z;
}
