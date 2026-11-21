/* backend.md chapter 10 and aarch64.md chapter 10, the data of a list
   with designators, 6.7.11p18 to p20: a designator moves the walk to
   the subobject it names and the list goes on from the next one, a
   later value of a subobject overrides an earlier one, and an array of
   unknown size takes its size from its highest index, p23.  The data
   is the bytes the list leaves, in the order of their offsets, as a
   list without designators writes them.

   On x86-64 and on aarch64 the listing is the same text but for the
   alignment of the arrays of 16 bytes or more: the literals' block,
   .Ls0 "Jan" and .Ls1 "Dec", numbered in the order of the offsets that
   point to them; then table, [4] then [1] and [2], as .zero 4, .long
   2, .long 3, .zero 4, .long 5 and .zero 4; sparse, of unknown size,
   as .size sparse, 40, .zero 36 and .long 1; again, whose [0] = 9
   overrides the 1, as .long 9, 2 and 3; corner as .zero 4 and .long
   1; line, to.x, to.y after it and then from.y, as .zero 4, .long 2,
   3 and 4; points, [1].x, [1].y after it and [0].y, as .zero 4, .long
   6, .long 4, .long 5 and .zero 8; lines, [1] braced, then [0].to.y
   and [1].from.x, which keeps the 7 of [1].to.x, as .zero 12, .long
   8, .long 9, .zero 4, .long 7 and .zero 4; number, whose c
   overrides the whole union, as .byte 120 and .zero 3; named, whose
   string "abc" is overridden at [1] and then at [2], as .byte 97, 88
   and 89, .zero 1 and .long 5; and months as .quad .Ls0, .zero 80
   and .quad .Ls1.  An array of 16 bytes or more is under .p2align 4
   on x86-64, which the psABI asks of a global array, and under its
   element's alignment on aarch64. */
struct point
{
        int x, y;
};

struct line
{
        struct point from, to;
};

struct named
{
        char name[4];
        int n;
};

union number
{
        int i;
        char c;
};

int table[6] = { [4] = 5, [1] = 2, 3 };
int sparse[] = { [9] = 1 };
int again[3] = { 1, 2, 3, [0] = 9 };
struct point corner = { .y = 1 };
struct line line = { .to.x = 3, 4, .from.y = 2 };
struct point points[3] = { [1].x = 4, 5, [0].y = 6 };
struct line lines[2] = { [1] = { .to = { 7 } }, [0].to.y = 8, [1].from.x = 9 };
union number number = { .i = -1, .c = 'x' };
struct named named = { .name = "abc", .name[1] = 'X', 'Y', .n = 5 };
char *months[12] = { [11] = "Dec", [0] = "Jan" };
