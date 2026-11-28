/* A bit-field's width is not negative, 6.7.3.2p4. */
struct s
{
        int f : -1;
};
