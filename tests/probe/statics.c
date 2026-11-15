/* The statics probe: static objects and functions, in rcc's C.  For
   each integer type, at file scope, f_N, zero, fi_N, initialized, fp_N,
   a null pointer, and fpi_N, a pointer to fi_N; in blocks, a zero
   object and an initialized one, a pointer and a pointer to a block's
   object.  Each is read and written through the functions here, since
   no other file names it.  Then the forms of an initializer at file
   scope and in a block, a block's object that counts the calls of its
   function, objects of one name in two functions and in two blocks of
   one function, a block's object that hides a file's, a static
   function called by name and through a pointer, and the declarations
   of a name extern after static.  See statics.h. */

#include "statics.h"

/* The objects at file scope: zero, initialized, and the two pointers.
   A tentative definition written twice is one object, 6.9.3p2. */
#define OBJECTS(T, N, V)     \
        static T f_##N;      \
        static T f_##N;      \
        static T fi_##N = V; \
        static T *fp_##N;    \
        static T *fpi_##N = &fi_##N;

INITIAL(OBJECTS)

/* Each object at file scope read, written and added to in memory and
   for its value; its address returned; the pointer read, written and
   dereferenced. */
#define FILE_SCOPE(T, N, V)         \
        long read_f_##N(void)       \
        {                           \
                return f_##N;       \
        }                           \
        void write_f_##N(long v)    \
        {                           \
                f_##N = v;          \
        }                           \
        long add_f_##N(long v)      \
        {                           \
                return f_##N += v;  \
        }                           \
        long read_fi_##N(void)      \
        {                           \
                return fi_##N;      \
        }                           \
        T *address_fi_##N(void)     \
        {                           \
                return &fi_##N;     \
        }                           \
        T *pointer_fpi_##N(void)    \
        {                           \
                return fpi_##N;     \
        }                           \
        long deref_fpi_##N(void)    \
        {                           \
                return *fpi_##N;    \
        }                           \
        int null_fp_##N(void)       \
        {                           \
                return fp_##N == 0; \
        }                           \
        void set_fp_##N(T *q)       \
        {                           \
                fp_##N = q;         \
        }                           \
        long store_fp_##N(long v)   \
        {                           \
                *fp_##N = v;        \
                return *fp_##N;     \
        }

INITIAL(FILE_SCOPE)

/* The objects of a block: swap_b_N() returns what its zero object held
   and stores v in it, step_bi_N() returns its initialized object and
   steps it, keep_bp_N() returns its pointer and stores q in it, and
   block_pointer_N() reads its object through its pointer to it, an
   address constant of a block's object. */
#define BLOCK_SCOPE(T, N, V)           \
        long swap_b_##N(long v)        \
        {                              \
                static T b;            \
                long old = b;          \
                                       \
                b = v;                 \
                return old;            \
        }                              \
        long step_bi_##N(void)         \
        {                              \
                static T bi = V;       \
                                       \
                return bi++;           \
        }                              \
        T *keep_bp_##N(T *q)           \
        {                              \
                static T *bp;          \
                T *old = bp;           \
                                       \
                bp = q;                \
                return old;            \
        }                              \
        long block_pointer_##N(long v) \
        {                              \
                static T bx = V;       \
                static T *bpx = &bx;   \
                long old = *bpx;       \
                                       \
                bx = v;                \
                return old;            \
        }

INITIAL(BLOCK_SCOPE)

/* A static function, and the static objects its address initializes. */
static long helper(long x);

static helper_t handler = helper;

static long
helper(long x)
{
        return x * 3 + 1;
}

/* The forms of an initializer at file scope, 6.7.11: a list of an
   array of unknown size, a list with its braces left out in part, a
   string for an array of char with room for its zero and without, and
   for one of unknown size, a pointer to a string, an array of them, a
   scalar in braces, the empty initializer, and address constants of an
   element and of a string. */
static int table[] = { 1, 2, 3 };
static int grid[2][3] = { { 1 }, 4, 5 };
static char word[8] = "abc";
static char exact[3] = "abc";
static char sized[] = "abcd";
static char *name = "Jan";
static char *names[] = { "Feb", "Mar" };
static int braced = { 5 };
static int empty[3] = { };
static int *element = &table[2];
static char *suffix = "Apr" + 1;

long
file_table(int i)
{
        return table[i];
}

long
file_grid(int i, int j)
{
        return grid[i][j];
}

long
file_word(int i)
{
        return word[i];
}

long
file_exact(int i)
{
        return exact[i];
}

long
file_sized(int i)
{
        return sized[i];
}

long
file_name(int i)
{
        return name[i];
}

long
file_names(int i, int j)
{
        return names[i][j];
}

long
file_braced(void)
{
        return braced;
}

long
file_empty(int i)
{
        return empty[i];
}

long
file_element(void)
{
        return *element;
}

long
file_suffix(int i)
{
        return suffix[i];
}

long
file_sizes(void)
{
        return sizeof table * 100 + sizeof sized * 10 + sizeof names;
}

/* The same forms in a block, read by one function each, which the main
   calls before anything writes them. */
long
block_table(int i)
{
        static int table[] = { 6, 7, 8, 9 };

        return table[i];
}

long
block_grid(int i, int j)
{
        static int grid[2][3] = { 1, 2, { 3 } };

        return grid[i][j];
}

long
block_word(int i)
{
        static char word[6] = "xyz";

        return word[i];
}

long
block_exact(int i)
{
        static char exact[2] = "pq";

        return exact[i];
}

long
block_names(int i, int j)
{
        static char *names[] = { "May", "Jun", "Jul" };

        return names[i][j];
}

long
block_braced(void)
{
        static long braced = { -9 };

        return braced;
}

long
block_empty(int i)
{
        static long empty[2] = { };

        return empty[i];
}

long
block_element(void)
{
        static int *element = &table[1];

        return *element;
}

long
block_sizes(void)
{
        static int table[] = { 6, 7, 8, 9 };
        static char sized[] = "ab";

        return sizeof table * 10 + sizeof sized;
}

/* A block's object counts the calls of its function: it keeps its
   value from call to call, 6.2.4p3. */
int
counter(void)
{
        static int calls;

        return ++calls;
}

/* One name in two functions, and in two blocks of one function: four
   objects. */
int
first(int v)
{
        static int same;
        int old = same;

        same = v;
        return old;
}

int
second(int v)
{
        static int same = 1000;
        int old = same;

        same = v;
        return old;
}

int
blocks(int which, int v)
{
        if (which) {
                static int same = 2000;
                int old = same;

                same = v;
                return old;
        }
        else {
                static int same;
                int old = same;

                same = v;
                return old;
        }
}

/* A block's object hides a file's of its name, and is hidden by an
   inner block's: shade is 1 at file scope, 2 in hides(), 3 in its
   inner block. */
static int shade = 1;

int
file_shade(void)
{
        return shade;
}

int
hides(void)
{
        static int shade = 2;
        int r = shade;

        {
                static int shade = 3;

                r = r * 10 + shade++;
        }

        return r * 10 + shade++;
}

/* The static function by name, through a pointer, and its address:
   the object handler holds it, and the main calls it through the
   address returned. */
long
call_helper(long x)
{
        return helper(x);
}

long
call_through(long x)
{
        helper_t f = helper;

        return f(x) + handler(x);
}

helper_t
helper_address(void)
{
        return helper;
}

int
same_helper(void)
{
        return handler == helper && handler == &helper;
}

/* A name statics-lib.c gives an object and a function of its own:
   each file reads its own. */
static int shared = 1;

static int
same_name(void)
{
        return 10;
}

int
own_shared(void)
{
        return shared;
}

int
own_same_name(void)
{
        return same_name();
}

/* extern after static keeps internal linkage, 6.2.2p4, and so does a
   function's declaration without a storage class, p5: again and
   twice are each one entity of this file. */
static int again = 4;
extern int again;

static long twice(long x);
extern long twice(long x);

long
twice(long x)
{
        return x + x;
}

long
read_again(void)
{
        return again + twice(again);
}
