/* A record holds no instance of itself, 6.7.3.2p3. */
struct s
{
        struct s x;
};
