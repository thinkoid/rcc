/* The lists the pointers probe is generated from.  pointers.c makes
   functions of each entry, compiled by rcc and, as the reference, by
   clang -O0 under the prefix ref_.  pointers-lib.c holds the functions
   they call, compiled by clang alone and linked under both names.
   pointers-main.c calls both builds over a table of values and
   compares the results and the bytes of the memory they wrote.
   Pointer arithmetic and the subscript, docs/aarch64.md 6.3's rows of
   p[i], &p[i], p + n and p - q and the paragraphs on the index under
   the table; the frame of 5.3, depth of 6.2, G6 and G7 of chapter 9. */

/* the integer types, of an index, and the letter each takes in a
   function's name */
#define NARROW(X)                                                       \
    X(char, c) X(signed char, sc) X(unsigned char, uc)                  \
    X(short, s) X(unsigned short, us)

#define WIDE(X) X(int, i) X(unsigned, u) X(long, l) X(unsigned long, ul)

#define TYPES(X) NARROW(X) WIDE(X)

/* the same list crossed with one entry of another, the pointee or the
   element */
#define INDEXES(X, E, T, S)                                             \
    X(E, T, S, char, c) X(E, T, S, signed char, sc)                     \
    X(E, T, S, unsigned char, uc) X(E, T, S, short, s)                  \
    X(E, T, S, unsigned short, us) X(E, T, S, int, i)                   \
    X(E, T, S, unsigned, u) X(E, T, S, long, l)                         \
    X(E, T, S, unsigned long, ul)

/* The pointees of pointer arithmetic: a name, a type and the
   declarator suffix that makes a parameter a pointer to it.  Sizes 1,
   2, 4 and 8, which an addressing form scales by, and the rows of
   arrays of arrays, of 3, 12 and 40 bytes, which none does. */
#define POINTEES(X)                                                     \
    X(c, char, []) X(s, short, []) X(i, int, []) X(l, long, [])         \
    X(r3, char, [][3]) X(r12, int, [][3]) X(r40, long, [][5])

/* the scalar pointees alone, for the steps of a pointer in memory and
   the walks */
#define SCALARS(X)                                                      \
    X(c, char, []) X(s, short, []) X(i, int, []) X(l, long, [])

/* The elements of a subscript, one of each size in each signedness:
   the load's extension changes the instruction, not the address. */
#define ELEMENTS(X)                                                     \
    X(sc, signed char, []) X(uc, unsigned char, [])                     \
    X(s, short, []) X(us, unsigned short, [])                           \
    X(i, int, []) X(u, unsigned, []) X(l, long, [])                     \
    X(ul, unsigned long, [])

/* the size of what p points to, a constant the parser folds */
#define SIZE ((long) sizeof *p)

/* Constant n of p + n and p += n: 0, 1 and -1, and the counts whose
   product with the size sits on each side of what an add immediate
   holds, 4095, a multiple of 4096 up to 0xfff000, beyond it, beyond
   32 bits, and their negatives.  For a size that is no scale the
   product is the multiple of the size nearest the bound. */
#define KONSTS(X, E, T, S)                                              \
    X(E, T, S, 0, 0) X(E, T, S, 1, 1) X(E, T, S, m1, -1)                \
    X(E, T, S, below, 4095 / SIZE) X(E, T, S, above, (4095 + SIZE) / SIZE) \
    X(E, T, S, page, 4096 / SIZE) X(E, T, S, next, (4096 + SIZE) / SIZE) \
    X(E, T, S, pages, 0x5000 / SIZE) X(E, T, S, top, 0xfff000 / SIZE)   \
    X(E, T, S, beyond, 0x1000000 / SIZE)                                \
    X(E, T, S, past, (0xfff000 + SIZE) / SIZE)                          \
    X(E, T, S, wide, 0x80000000 / SIZE)                                 \
    X(E, T, S, huge, 0x123456789 / SIZE)                                \
    X(E, T, S, mbelow, -(4095 / SIZE))                                  \
    X(E, T, S, mabove, -((4095 + SIZE) / SIZE))                         \
    X(E, T, S, mpage, -(4096 / SIZE)) X(E, T, S, mtop, -(0xfff000 / SIZE)) \
    X(E, T, S, mbeyond, -(0x1000000 / SIZE))                            \
    X(E, T, S, mhuge, -(0x123456789 / SIZE))

/* A constant subscript, on each side of the load's reach: the scaled
   form reaches 4095 elements, the unscaled one -256 to 255 bytes. */
#define KSUBS(X, E, T, S)                                               \
    X(E, T, S, 0, 0) X(E, T, S, 1, 1) X(E, T, S, 255, 255)              \
    X(E, T, S, 256, 256) X(E, T, S, 4095, 4095) X(E, T, S, 4096, 4096)  \
    X(E, T, S, m1, -1) X(E, T, S, m256, -256) X(E, T, S, m257, -257)

/* a displacement in bytes through a cast to a wider element, no
   multiple of the size */
#define KBYTES(X, E, T, S)                                              \
    X(E, T, S, 1, 1) X(E, T, S, 3, 3) X(E, T, S, 5, 5)                  \
    X(E, T, S, 255, 255) X(E, T, S, 257, 257) X(E, T, S, 4097, 4097)    \
    X(E, T, S, m1, -1) X(E, T, S, m3, -3) X(E, T, S, m255, -255)        \
    X(E, T, S, m257, -257)

/* eight long parameters, the convention's registers on aarch64 */
#define P8(T) T a, T b, T c, T d, T e, T f, T g, T h
