/* shape.md chapter 4: a member access is the dereference of the
   record's address plus the member's offset, *(int *) ((char *) p +
   8) for p->m, and each generator reads the constant into the
   operand.  On x86-64 every shape here is one operand: p->m 8(%rax),
   s.m the slot -136(%rbp), g.m the label g+8(%rip), p->in.i 20(%rax)
   with the two offsets one, p->a[i] 32(%rax,%rcx,4), p->a[3] 44(%rax),
   p->t[i].i 68(%rax,%rcx,1) after the imulq $16 of an element no scale
   covers, p[i].m and tab[i].m 8(%rax,%rcx,1) after imulq $144, the last
   after the leaq of the label, which takes no index; p->m++ and p->m +=
   v the memory forms, addl $1, 8(%rax); &p->m one leaq 8(%rax), and
   &p->a[3], &p->t[2].h, &s.a[3] and &g.a[3] the one leaq 44(%rax), leaq
   98(%rax), leaq -100(%rbp) and leaq g+44(%rip); a register pointer's
   r->m 8(%rbx), and its walk r = r->next movq 128(%rbx), %rbx; a
   union's members each at 0; and p->f(x) the call through the member's
   load.  An address held across a call in the right operand keeps its
   constants in the access's displacement and its base and index in one
   register, spilled once: held_member(), p->t[i].i = p->f(x), is the
   imulq and addq %rcx, %rax before the call and movl %eax, 68(%rcx)
   after it, the 64 and the 4 one displacement, and held_element(),
   p->a[i] += p->f(x), leaq (%rax,%rcx,4), %rax before it and addl
   %eax, 32(%rcx).  On aarch64 p->m is [x0, #8], s.m [x29, #d],
   g.m [x16, :lo12:g+8], p->in.i [x0, #20], p->a[i] the add of
   the offset and [x0, x1, lsl #2], since no form holds an index
   and a displacement, and a register pointer's [x19, #8]; a member
   array's element by a constant, p->a[3] and p->t[2].h, one access,
   the member array's offset and the element's one displacement, [x0,
   #44] and [x0, #98], and by a variable index with a member after it,
   p->t[i].i, adds the index to p shifted by the element's 16 bytes,
   add x0, x0, x1, lsl #4, and the member array's 64 and the member's
   4 are the access's displacement, [x0, #68].
   p[i].m adds the product to p and reads [x0, #8], and tab[i].m
   on the label is adrp x0, tab+8, the product's add and ldr w0,
   [x0, :lo12:tab+8], with no add of the page's offset; p[i].a[3] and
   tab[i].a[3], a member array's element under the record's index, read
   [x0, #44] and [x0, :lo12:tab+44] past the product's add, and *(int *)
   ((char *) &tab[i] + 2), whose 2 is no multiple of the int's 4 bytes,
   keeps adrp x0, tab and add x0, x0, :lo12:tab before the product's add
   and [x0, #2].  The addresses &p->a[3] and &p->t[2].h are one add,
   add x0, x0, #44 and add x0, x0, #98, &s.a[3] the slot's add x0,
   x29, #60, and &g.a[3] the label's pair, adrp x0, g+44 and add x0,
   x0, :lo12:g+44.  held_member() adds the index shifted by 4 before
   the call and stores str w0, [x1, #68] after it, and held_element()
   add x0, x0, x1, lsl #2 before it, then ldr, add and str at [x2,
   #32], the member array's 32 the two accesses' displacement. */
struct in
{
        char c;
        short h;
        int i;
        long l;
};

struct rec
{
        long pad;
        int m;
        struct in in;
        int a[8];
        struct in t[4];
        struct rec *next;
        int (*f)(int);
};

union freq
{
        unsigned short freq;
        unsigned char lo;
};

struct rec g;
struct rec tab[10];

int
read_ptr(struct rec *p)
{
        return p->m;
}

void
write_ptr(struct rec *p, int v)
{
        p->m = v;
}

void
step_ptr(struct rec *p)
{
        p->m++;
}

void
add_ptr(struct rec *p, int v)
{
        p->m += v;
}

int
read_slot(void)
{
        struct rec s;

        s.m = 1;
        return s.m;
}

int
read_label(void)
{
        return g.m;
}

int
nested(struct rec *p)
{
        return p->in.i;
}

int
element(struct rec *p, long i)
{
        return p->a[i];
}

int
element_constant(struct rec *p)
{
        return p->a[3];
}

int
element_member(struct rec *p, long i)
{
        return p->t[i].i;
}

int
element_constant_member(struct rec *p)
{
        return p->t[2].h;
}

int
indexed(struct rec *p, long i)
{
        return p[i].m;
}

int
indexed_label(long i)
{
        return tab[i].m;
}

int *
address(struct rec *p)
{
        return &p->m;
}

int
in_register(struct rec *q)
{
        register struct rec *r = q;

        return r->m;
}

struct rec *
last(struct rec *q)
{
        register struct rec *r = q;

        while (r->next)
                r = r->next;

        return r;
}

int
pun(union freq *p)
{
        p->freq = 0x1234;
        return p->lo;
}

int
call(struct rec *p, int x)
{
        return p->f(x);
}

int *
element_address(struct rec *p)
{
        return &p->a[3];
}

short *
element_member_address(struct rec *p)
{
        return &p->t[2].h;
}

void use(int *);

void
element_address_slot(void)
{
        struct rec s;

        use(&s.a[3]);
}

int *
element_address_label(void)
{
        return &g.a[3];
}

void
held_member(struct rec *p, long i, int x)
{
        p->t[i].i = p->f(x);
}

void
held_element(struct rec *p, long i, int x)
{
        p->a[i] += p->f(x);
}

/* A member array's element under the record's index, p[i].a[3] and
   tab[i].a[3], and a member's bytes at an offset no multiple of the
   access's size under the index on a label, *(int *) ((char *) &tab[i]
   + 2).  On x86-64 the first two are movl 44(%rax,%rcx,1), %eax after
   the imulq $144, the second after the leaq of the label, and the
   third movl 2(%rax,%rcx,1), %eax after the same. */
int
element_indexed(struct rec *p, long i)
{
        return p[i].a[3];
}

int
element_indexed_label(long i)
{
        return tab[i].a[3];
}

int
indexed_label_odd(long i)
{
        return *(int *) ((char *) &tab[i] + 2);
}

/* A record's member under a variable index at a size that is a power of
   two: records of 16, 32 and 4096 bytes, p[i].b by an int index with
   the pointer written first and n[p].c by a long index with the index
   written first in sixteen(), thirty_two() and page(), the other order
   of each in sixteen_first(), thirty_two_first() and page_first(), and
   p[code()].b by an unsigned call's result in sixteen_unsigned().  On
   x86-64, whose forms scale by 8 at most, each index is multiplied:
   movslq -12(%rbp), %rcx; imulq $16, %rcx, %rcx and movl
   8(%rax,%rcx,1), %eax for p[i].b, imulq $16, -24(%rbp), %rcx, the long
   in memory the multiply's source, then addq -8(%rbp), %rcx and addl
   12(%rcx), %eax for n[p].c, and the same with imulq $32 and imulq
   $4096; sixteen_unsigned() movl %eax, %eax, the call's result
   zero-extended, before imulq $16, %rax, %rax.  On aarch64 the add
   shifts each index, and the member is the access's displacement: at 16
   bytes an int by the extended operand, ldr w1, [x29, #24]; add x0, x0,
   w1, sxtw #4; ldr w0, [x0, #8], a long whole, add x1, x2, x1, lsl #4;
   ldr w1, [x1, #12], and the unsigned call's result add x1, x1, w0,
   uxtw #4, whose upper half the convention leaves undefined; above 16
   the index whole, extended by its load, ldrsw x1, [x29, #24]; add x0,
   x0, x1, lsl #5 and lsl #12, then [x0, #24] and [x1, #28], [x0, #4088]
   and [x1, #4092]. */
struct r16
{
        long a;
        int b, c;
};

struct r32
{
        long a[3];
        int b, c;
};

struct page
{
        char pad[4088];
        int b, c;
};

unsigned code(void);

long
sixteen(struct r16 *p, int i, long n)
{
        return p[i].b + n[p].c;
}

long
sixteen_first(struct r16 *p, int i, long n)
{
        return i[p].b + p[n].c;
}

int
sixteen_unsigned(struct r16 *p)
{
        return p[code()].b;
}

long
thirty_two(struct r32 *p, int i, long n)
{
        return p[i].b + n[p].c;
}

long
thirty_two_first(struct r32 *p, int i, long n)
{
        return i[p].b + p[n].c;
}

long
page(struct page *p, int i, long n)
{
        return p[i].b + n[p].c;
}

long
page_first(struct page *p, int i, long n)
{
        return i[p].b + p[n].c;
}
