/* A member has no incomplete type, 6.7.3.2p3. */
struct t;
struct s
{
        struct t x;
};
