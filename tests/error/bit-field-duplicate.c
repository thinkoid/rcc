/* A member of the name of a bit-field declared before it: a struct has
   one name space for its members, bit-fields among them, 6.7.3.2p1 and
   6.2.3p1, so a is a second member of one name. */
struct flags
{
        int a : 3;
        int a;
};
