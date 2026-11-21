/* A bit-field wider than its type: the width does not exceed the width
   of an object of the type, 6.7.3.2p4, 32 bits for int. */
struct flags
{
        int z : 33;
};
