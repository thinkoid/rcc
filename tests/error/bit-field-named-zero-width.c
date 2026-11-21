/* A bit-field of width zero with a name: the declaration of a width of
   zero has no declarator, 6.7.3.2p4. */
struct flags
{
        unsigned a : 1;
        unsigned y : 0;
};
