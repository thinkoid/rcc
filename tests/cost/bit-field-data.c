/* shape.md chapter 4, the data of objects whose records hold
   bit-fields: each field's constant, of its declared type cut to its
   width (G2), in the bytes its bits lie in, from the lowest bit up,
   each byte a .byte of the bits of every field the list gives in it;
   the bytes no value holds, the unnamed fields' and the padding among
   them, are zero, 6.7.11p10, p11.  A later value of a field clears the
   field's own bits first and no neighbour's, p20.  A union takes its
   first named member.

   On x86-64 and on aarch64 the listing is the same text: job as .long
   1, .byte 3, 0, 2 and 26 and .quad 0, nprocs 3 in two bytes, state 2
   and the flags jobctl, used and changed, bits 1, 3 and 4 of the last;
   small as .byte 237, 100, 255, 255, 255, 255, 127 and .zero 1, a 5,
   f -3 at bit 3, u 100 and l -2 at bit 15; file as .long 7, .byte 255,
   255, 255 and 120 and .quad 9, flags2 -1 in the container's three
   lower bytes and backup in its fourth; again as .byte 248, 128, 0, 0,
   0, 0, 0 and .zero 1, f set to 17 and then to -1, which clears its
   bits first and keeps l's 1; one as .byte 5 and .zero 3; and skip as
   .byte 120, .zero 3, .byte 2 and 99 and .zero 2, f 15 at bit 3 past
   an unnamed field of 3 bits, g 2 in the next int past one of width 0,
   and c in the byte after g's.  The bytes of .data are those clang
   writes for the file on each target. */
struct job
{
        int stop;
        unsigned nprocs : 16, state : 8, sigint : 1, jobctl : 1, waited : 1,
            used : 1, changed : 1;
        struct job *prev;
};

struct job job = { 1, 3, 2, 0, 1, 0, 1, 1, 0 };

struct small
{
        unsigned a : 3;
        int f : 5;
        unsigned u : 7;
        long l : 40;
};

struct small small = { 5, -3, 100, -2 };

struct file
{
        int fileno;
        int flags2 : 24;
        char backup[1];
        long off;
};

struct file file = { 7, -1, { 'x' }, 9 };

struct small again = { .f = 17, .l = 1, .f = -1 };

union one
{
        int f : 3;
        char c;
} one = { 5 };

struct skip
{
        int : 3;
        int f : 4;
        int : 0;
        int g : 2;
        char c;
} skip = { 15, 2, 'c' };
