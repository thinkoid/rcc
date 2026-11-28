/* A bit-field's name is a member's, 6.7.3.2p1. */
struct s
{
        int a;
        int a : 3;
};
