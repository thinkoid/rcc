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
   %eax, 32(%rcx).  On aarch64 p->m is [x0, #8], s.m [x29, #d], g.m [x16,
   :lo12:g+8], p->in.i [x0, #20], p->a[i] the add of the offset and [x0,
   x1, lsl #2], since no form holds an index and a displacement, and a
   register pointer's [x19, #8]; a member array's element by a constant,
   p->a[3] and p->t[2].h, one access, the member array's offset and the
   element's one displacement, [x0, #44] and [x0, #98], and by a
   variable index with a member after it, p->t[i].i, adds the index's
   product to p, and the member array's 64 and the member's 4 are the
   access's displacement, [x0, #68].  The addresses &p->a[3] and
   &p->t[2].h are one add, add x0, x0, #44 and add x0, x0, #98, &s.a[3]
   the slot's add x0, x29, #60, and &g.a[3] the label's pair, adrp x0,
   g+44 and add x0, x0, :lo12:g+44.  held_member() adds the index's
   product before the call and stores str w0, [x1, #68] after it, and
   held_element() add x0, x0, #32 and add x0, x0, x1, lsl #2 before
   it, then ldr, add and str at [x2]. */
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
