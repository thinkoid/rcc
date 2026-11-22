/* guarantees.md, a volatile bit-field is accessed as any other: a read
   is one load of its unit, a store of a value one load and one store
   of it, and a store of 0 or of all ones the one compound assignment
   on it, since c0 drops volatile and c1 writes every access the source
   writes (shape.md chapter 5).  status() reads ready and count, which
   fills its byte, arm() stores 1 into ready, and reset() 0 into count
   and a value into level.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program without volatile.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
struct device
{
        volatile unsigned ready : 1, level : 5, : 2;
        volatile unsigned count : 8;
};

int
status(struct device *d)
{
        return d->ready + d->count;
}

void
arm(struct device *d)
{
        d->ready = 1;
}

void
reset(struct device *d, int level)
{
        d->count = 0;
        d->level = level;
}
