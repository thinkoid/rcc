/* backend.md chapter 10, aarch64.md chapter 10 and shape.md chapter 5,
   the data of objects of structs and unions: each member the list
   gives at its offset, in declaration order, in the directive of its
   form at its width, and a .zero for each run of bytes no member
   holds, padding included, so that the bytes written are the object's
   size, 6.7.11p11, p18, p21.
   A list without braces of its own for a member that is a record or
   an array takes as many initializers as the member's own members or
   elements do; a union takes its first member alone; a string
   initializes a member that is an array of char with its own bytes.

   On x86-64 and on aarch64 the listing is the same text but for the
   alignment of the arrays of 16 bytes or more, .p2align 4 on x86-64
   and their element's on aarch64: table, an array of records with a
   function pointer, as four .short and a .quad for each element,
   .quad 0 for the null pointer and .quad fast and .quad slow for the
   functions; codes, two records of two unions, braced in full and
   elided, as .short 12, 8, 140 and 8; names, of unknown size, as .size
   names, 24, .string "one", .zero 4, .long 1, then .ascii "three",
   which fills its array, .zero 3 and .long 3; segment, elided, as
   .long 1 to 4; partial as .long 7 and .zero 12; nested as .long 1,
   .zero 4, .byte 97, .zero 7, .quad 2, .short 3 and .zero 6; either as
   .byte 120 and .zero 7; second as .quad segment+8; and third as .quad
   table+32.  The bytes of .data and its relocations are those clang
   writes for the file on each target. */
struct config
{
        short good, lazy, nice, chain;
        int (*func)(int);
};

int fast(int);
int slow(int);

struct config table[4] = {
        { 0, 4, 8, 32, 0 },
        { 4, 4, 8, 4, fast },
        { 4, 5, 16, 8, fast },
        { 8, 16, 128, 128, slow },
};

struct code
{
        union
        {
                unsigned short freq, code;
        } fc;
        union
        {
                unsigned short dad, len;
        } dl;
};

struct code codes[2] = { { { 12 }, { 8 } }, 140, 8 };

struct name
{
        char text[5];
        int count;
};

struct name names[] = { "one", 1, { "three", 3 } };

struct point
{
        int x, y;
};

struct segment
{
        struct point from, to;
} segment = { 1, 2, 3, 4 };

struct segment partial = { 7 };

struct nested
{
        int tag;
        struct inner
        {
                char c;
                long l;
        } in;
        short s;
} nested = { 1, { 'a', 2 }, 3 };

union either
{
        char c;
        long l;
} either = { 'x' };

int *second = &segment.to.x;
struct config *third = table + 2;
