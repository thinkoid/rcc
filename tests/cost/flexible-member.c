/* shape.md chapter 3: a flexible array member, 6.7.3.2p20, stands at
   its element's alignment past the other members and adds no size, so
   struct packet is 4 bytes and its bytes at 4, struct words is 8 bytes
   and its words at 8, and sizes() is the constant 12.  An element of
   the member is the member's offset plus the index, as of any member
   array (member.c): on x86-64 byte() reads movzbl 4(%rax,%rcx,1),
   put() writes movb %dl, 4(%rax,%rcx,1) and then the header's length
   at (%rax), word() reads movq 8(%rax,%rcx,8), and first(), the
   member's value, its address, is leaq 4(%rax).  On aarch64, where no
   form holds an index and a displacement, the member's offset is an
   add before the access: byte() add x0, x0, #4 and ldrb w0, [x0, x1],
   put() strb w2, [x0, x1] and str w1, [x0], word() add x0, x0, #8 and
   ldr x0, [x0, x1, lsl #3], and first() the add x0, x0, #4 alone. */
struct packet
{
        int length;
        unsigned char bytes[];
};

struct words
{
        char tag;
        long words[];
};

int
length(struct packet *p)
{
        return p->length;
}

int
byte(struct packet *p, long i)
{
        return p->bytes[i];
}

void
put(struct packet *p, long i, int c)
{
        p->bytes[i] = c;
        p->length = i + 1;
}

long
word(struct words *w, long i)
{
        return w->words[i];
}

unsigned char *
first(struct packet *p)
{
        return p->bytes;
}

long
sizes(void)
{
        return sizeof(struct packet) + sizeof(struct words);
}
