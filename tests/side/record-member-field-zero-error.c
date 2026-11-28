/* A named bit-field's width is not zero, 6.7.3.2p4. */
struct s
{
        int f : 0;
};
