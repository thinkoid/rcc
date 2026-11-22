/* shape.md chapter 3: a type rcc does not use yet that a means makes
   from another type is one type each time the same means makes it from
   the same type, so a typedef through it may be declared again, and a
   function declared again with it.  The means are the marking
   attributes with their arguments, in their order, mode (word) here,
   spelled __mode__ (__word__) the second time, and mode (word) and
   aligned (8) together; _Atomic, the qualifier or the type specifier;
   and an array size rcc cannot compute, the size of a packed struct.
   Each declaration writes nothing.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program without the second declaration of each
   name.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
typedef int word_t __attribute__((mode(word)));
typedef int word_t __attribute__((__mode__(__word__)));

typedef _Atomic int counter_t;
typedef _Atomic int counter_t;

void bump(_Atomic(int) *count);
void bump(_Atomic(int) *count);

struct regs
{
        long r[2];
        int live;
} __attribute__((packed));

typedef long regs_t[sizeof(struct regs)];
typedef long regs_t[sizeof(struct regs)];

typedef int aligned_word_t __attribute__((mode(word), aligned(8)));
typedef int aligned_word_t __attribute__((mode(word), aligned(8)));

word_t *
first(word_t *w)
{
        return w;
}
