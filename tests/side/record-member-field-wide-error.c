/* A bit-field's width does not exceed its type's, 6.7.3.2p4. */
struct s
{
        int f : 33;
};
