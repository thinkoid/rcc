/* A member declaration has no storage class, typedef among them,
   6.7.3.2p1. */
struct s
{
        typedef int a;
};
