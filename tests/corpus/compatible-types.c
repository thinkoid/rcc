/* Declarations of one entity whose types are compatible and not the
   same, each taking the composite type, 6.2.7p3, p5, and pointers of
   one machine type and two C types converted by the cast 6.5.17.2p1
   asks for.  f is declared with a pointer to an array of unknown size
   and then of three ints, as 6.2.7p6's EXAMPLE writes it, and defined
   with the second.  count is declared of an enumerated type and then
   of its integer type, unsigned int, whose composite rcc takes as the
   integer type, as clang does (choices.md), so a third declaration of
   another enumeration of that integer type is compatible with it;
   total is declared of the two in the other order, of the same
   composite.  The arms of the conditional point to an array of
   unknown size and to one of three ints, and meet at the pointer to
   the composite, whose pointee has a size.  through_long_long() and
   through_shape() convert a pointer to long and a pointer to an
   enumerated type by a cast, and read through the pointer converted
   back, 6.3.2.3p7.  It exits 42 when each holds, or 100 plus the
   number of the first check that failed.

   Under rcc and under clang -O2 it exits 42.

   On x86-64 the listing must show f() returning movl $12, %eax, the
   size of the composite in pick() the same movl $12, %eax, and each
   cast in through_long_long() and through_shape() no instruction:
   p's load and its store to q, then q's load and the read through it,
   movq (%rax), %rax and movl (%rax), %eax.

   On aarch64 the listing must show mov w0, #12 in f() and in pick(),
   and no instruction for a cast: p's load and its store to q, then
   q's load and the read through it, ldr x0, [x0] and ldr w0, [x0]. */
enum color { red, green };
enum shape { round, square };

int f(int (*)[]);
int f(int (*)[3]);

enum color count;
unsigned int count;
enum shape count;

unsigned int total;
enum color total;
enum shape total;

int grid[3];
int (*any)[];
int (*rows)[3] = &grid;

int
f(int (*row)[3])
{
        return sizeof *row;
}

int
pick(int c)
{
        return sizeof *(c ? any : rows);
}

long
through_long_long(long *p)
{
        long long *q = (long long *) p;

        return *(long *) q;
}

enum color
through_shape(enum color *p)
{
        enum shape *q = (enum shape *) p;

        return *(enum color *) q;
}

int
main(void)
{
        long value = 5;
        enum color hue = green;

        count = square;
        total = green;

        if (f(&grid) != 12)
                return 101;

        if (pick(0) != 12)
                return 102;

        if (through_long_long(&value) != 5)
                return 103;

        if (through_shape(&hue) != green)
                return 104;

        if (count != 1)
                return 105;

        if (total != 1)
                return 106;

        return 42;
}
