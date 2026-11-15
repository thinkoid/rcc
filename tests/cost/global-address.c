/* backend.md chapter 10 and aarch64.md chapter 10, the data of an
   address constant, 6.6p9, p11, as an object's initializer: one .quad
   in .data, the symbol and its offset in bytes, which the linker
   resolves or leaves to the loader, alike for an object or a function
   the file defines and for one it only declares.  The offset is the
   constant times the pointee's size, as a subscript's is; a string
   literal an initializer points into is one more literal of the file,
   with its .Ls<n> label in the literals' block.  An integer constant
   cast to a pointer, and a null pointer, are the number; an address
   cast to an integer of 8 bytes is the address.

   On x86-64 and on aarch64 the listing is the same text but for two
   lines: the literals' block, .Ls0 and .Ls1, each .string "abc"; then
   .data, each pointer under .size 8 and .p2align 3: p as .quad n,
   element as .quad a+12, decay as .quad a, sum as .quad a+8,
   difference as .quad a+4, cell as .quad m+24, row as .quad m+16,
   bytes as .quad n+1, handler and handler_address as
   .quad on_signal, string as .quad .Ls0, suffix as .quad .Ls1+1, out
   as .quad stdout, put as .quad putchar, fixed as .quad 4096, null
   and void_null as .quad 0, and where as .quad n; then .bss, n, and a
   and m under .p2align 4 on x86-64, which the psABI asks of an array
   of 16 bytes or more, and under their element's .p2align 2 on
   aarch64. */
struct file;

int n;
int a[4];
int m[3][4];
extern struct file *stdout;
int putchar(int);
void on_signal(int);

int *p = &n;
int *element = &a[3];
int *decay = a;
int *sum = a + 2;
int *difference = &a[3] - 2;
int *cell = &m[1][2];
int (*row)[4] = &m[1];
char *bytes = (char *) &n + 1;
void (*handler)(int) = on_signal;
void (*handler_address)(int) = &on_signal;
char *string = "abc";
char *suffix = "abc" + 1;
struct file **out = &stdout;
int (*put)(int) = putchar;
int *fixed = (int *) 4096;
int *null = 0;
int *void_null = (void *) 0;
long where = (long) &n;
