/* shape.md chapter 5: a bit-field that fills its access unit, starting
   at the unit's bit 0 and as wide as it, is an object of the unit's
   integer type, of the field's signedness, so every operator on it is
   the one c1 writes for a member of that type: struct job's nprocs is
   an unsigned short at 4 and state an unsigned char at 6, and struct
   wide's lo is a signed char at 0, mid a short at 2 and whole a long
   at 8.  done() compares state, next() indexes by nprocs and steps it,
   set_state() stores into state and add() adds to nprocs; sum() reads
   the three of wide and step() steps mid and multiplies whole.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program with each record's fields declared as
   those members, unsigned short nprocs, unsigned char state and an
   unsigned char for the five flags, and signed char lo, short mid and
   long whole.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
struct job
{
        int stop;
        unsigned nprocs : 16, state : 8, sigint : 1, jobctl : 1, waited : 1,
            used : 1, changed : 1;
        struct job *prev;
};

int
done(struct job *p)
{
        return p->state == 2;
}

int
next(int *a, struct job *p)
{
        return a[p->nprocs++];
}

void
set_state(struct job *p, int s)
{
        p->state = s;
}

void
add(struct job *p, int n)
{
        p->nprocs += n;
}

struct wide
{
        int lo : 8;
        short mid : 16;
        long whole : 64;
};

long
sum(struct wide *p)
{
        return p->lo + p->mid + p->whole;
}

void
step(struct wide *p)
{
        --p->mid;
        p->whole *= 3;
}
