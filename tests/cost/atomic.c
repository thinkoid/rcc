/* _Atomic writes nothing (shape.md chapter 3): c0 drops the qualifier
   and the specifier, with a warning, so an _Atomic object is an object
   of its plain type and every access is the plain load and store.
   load() reads counter, bump() steps it, arm() stores 1 into ready and
   status() reads ready and level.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program without _Atomic, ready an unsigned.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
_Atomic int counter;

struct device
{
        _Atomic(unsigned) ready : 1;
        _Atomic unsigned level : 5;
};

int
load(void)
{
        return counter;
}

void
bump(void)
{
        counter++;
}

void
arm(struct device *d)
{
        d->ready = 1;
}

int
status(struct device *d)
{
        return d->ready + d->level;
}
