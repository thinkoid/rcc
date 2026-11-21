/* A bit-field of a negative width: the width is a nonnegative value,
   6.7.3.2p4. */
struct flags
{
        int w : -1;
};
