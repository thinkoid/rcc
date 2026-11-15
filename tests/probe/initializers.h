/* The shared declarations of the initializers probe.  initializers.c
   defines objects at file scope whose initializers are address
   constants and strings, compiled by rcc and, as the reference, by
   clang -O0 under the prefix ref_.  initializers-lib.c defines the
   objects and the functions initializers.c only declares, and objects
   of its own initialized the same ways, which initializers.c reads; it
   is compiled by clang alone and linked under both names.
   initializers-main.c checks the objects of both builds before
   anything writes them, and calls the functions of both.

   The data of an address constant, 6.6p9, p11, and of an array of char
   that a string initializes, 6.7.11p15, docs/backend.md chapter 10 and
   docs/aarch64.md chapter 10; an array declared without its size and
   completed by another declaration, 6.2.7p5, or at the end of the
   file, 6.9.3p2.  An address differs between the two builds, so the
   main compares what an address designates, or the address against
   the one its own build's object has. */

#define ARRAY   8
#define ROWS    3
#define COLUMNS 4

/* the strings the arrays and the pointers are initialized with */
#define WORD      "abc"
#define ESCAPED   "a\tb\n\"\\\001\377"
#define LONG_WORD "sixteen or more bytes"

/* the types of the pointers, one name each, so that one declaration
   names an object and its reference */
typedef int *int_pointer_t;
typedef int **int_pointer_pointer_t;
typedef long *long_pointer_t;
typedef char *char_pointer_t;
typedef void *void_pointer_t;
typedef int (*row_pointer_t)[COLUMNS];
typedef int (*unary_t)(int);
typedef void (*handler_t)(int);
typedef void (*thunk_t)(void);

/* initializers-lib.c defines an object, an array and a function the
   probe only declares, the array without its size, and sums the array
   the probe defines, which it declares without its size */
extern int lib_object;
extern int lib_array[];
int lib_triple(int v);

#define PROBE_ARRAY 4

extern int probe_array[];
long lib_total(void);

/* initializers-lib.c: objects initialized by clang, which the probe's
   code reads */
extern int *lib_element;
extern char *lib_suffix;
extern char lib_word[];
extern int (*lib_function)(int);
