/* A _Bool bit-field of width 2: the width does not exceed the width of
   an object of the type, 6.7.3.2p4, and a _Bool's width is 1 bit,
   though its object has a byte, the paragraph's footnote. */
struct flags
{
        _Bool b : 2;
};
