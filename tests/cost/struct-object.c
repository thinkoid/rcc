/* shape.md chapter 3 and backend.md chapters 5 and 10: objects of
   structs and unions, each of the size and the alignment its layout
   gives it, on both targets.  A local takes a slot at its record's
   alignment, its displacement the record's lowest address, and its
   address is the slot's, leaq -16(%rbp), %rax and add x0, x29, #20 for
   m.  A global the file defines is in .bss, under .p2align of its
   record's alignment and none for an alignment of 1, and an array of
   records of 16 bytes or more is aligned to 16 on x86-64, as an array
   of any element is.  An int read through a cast at an offset into the
   object, as a member's access reads it, is the slot's or the label's
   own operand at that offset, movl -8(%rbp), %eax and ldr w0, [x16,
   :lo12:wide+8]. */
struct bytes
{
        char a, b, c;
};

struct shorts
{
        short a, b, c;
};

struct mixed
{
        int a;
        char b;
        int c;
};

struct longs
{
        long a;
        char b;
        long c;
};

union either
{
        char c;
        long l;
};

struct bytes three;
struct shorts six;
struct mixed wide;
struct longs pairs[2];
static union either one;

void use(void *);

int
locals(void)
{
        struct bytes b;
        struct mixed m;
        union either e;

        use(&b);
        use(&m);
        use(&e);

        return *(int *) ((char *) &m + 8);
}

int
globals(void)
{
        use(&three);
        use(&one);

        return *(int *) ((char *) &wide + 8) + *(short *) ((char *) &six + 4);
}

long
element(void)
{
        return *(long *) ((char *) &pairs[1] + 16);
}

unsigned long
sizes(void)
{
        return sizeof(struct bytes) + sizeof six + sizeof wide + sizeof pairs +
               sizeof one;
}
