/* The shared declarations of the braces probe.  braces.c defines
   objects at file scope initialized by brace-enclosed lists, compiled
   by rcc and, as the reference, by clang -O0 under the prefix ref_.
   braces-lib.c defines objects of its own initialized by lists, which
   braces.c reads, and the function braces.c only declares; it is
   compiled by clang alone and linked under both names.  braces-main.c
   checks the objects of both builds before anything writes them, and
   calls the functions of both.

   The data of a list, 6.7.11p17 to p23, docs/backend.md chapter 10
   and docs/aarch64.md chapter 10: the elements in subscript order and
   the ones the list leaves out zero, the braces of a subaggregate left
   out in whole or in part, an array of unknown size completed by its
   list, an array of char initialized by a string, and each element a
   constant or an address constant.  An address differs between the
   two builds, so the main compares what an address designates, or the
   address against the one its own build's object has. */

#define ROWS    2
#define COLUMNS 3

/* the value of each guard, the object defined after an array: an
   array written one element short or long moves the guard's label
   and the value read there */
#define GUARD 0x5a

/* the types of the pointers, one name each, so that one declaration
   names an object and its reference */
typedef char *char_pointer_t;
typedef int *int_pointer_t;
typedef int (*unary_t)(int);
typedef int (*row_pointer_t)[COLUMNS];

/* braces-lib.c: a function braces.c's table points to, and objects
   clang initializes by lists, which braces.c reads */
int lib_triple(int v);

extern int lib_table[ROWS][COLUMNS];
extern int lib_elided[ROWS][COLUMNS];
extern char *lib_names[];
extern char lib_words[][4];

/* the records of the lists with designators */
struct designated_point
{
        int x, y;
};

struct designated_line
{
        struct designated_point from, to;
};

union designated_number
{
        int i;
        char c;
};

struct designated_named
{
        char name[4];
        int n;
};

/* the records of the lists that designate a member of an anonymous
   member, 6.7.3.2p15: an anonymous union's, an anonymous struct's, and
   one of an anonymous struct in an anonymous union */
struct designated_variant
{
        int tag;
        union
        {
                int i;
                char c;
        };
        int after;
};

struct designated_pair
{
        int tag;
        struct
        {
                int a, b;
        };
        int after;
};

struct designated_nested
{
        int tag;
        union
        {
                struct
                {
                        int x, y;
                };
                int whole;
        };
        int after;
};
