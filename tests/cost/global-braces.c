/* backend.md chapter 10 and aarch64.md chapter 10, the data of a
   brace-enclosed list, 6.7.11p17 to p23: the object's label, then each
   element the list gives in subscript order, in the directive of its
   form at its width, and a .zero for each run of elements the list
   leaves out, so that the bytes written are the object's size.  An
   object a list initializes is in .data whatever its values, {}
   among them.  A string literal an element points into is one more
   literal of the file, numbered after the functions' in the order
   met; a string that initializes an element that is an array of char
   is the element's own bytes.

   On x86-64 and on aarch64 the listing is the same text but for the
   alignment of the arrays of 16 bytes or more: the literals' block,
   .Ls0 to .Ls2, the strings "Jan", "Feb" and "abc"; then .data:
   counted, of unknown size, as .size counted, 12 and .long 1, 2 and
   3; padded as .long 1, .long 2 and .zero 12; table, braced in full,
   as .long 1 to 6; partly, { { 1 }, 4, 5 }, as .long 1, .zero 8,
   .long 4, .long 5 and .zero 4; rows, of unknown size, as .size rows,
   24, .long 1 to 4 and .zero 8; narrow, bytes, shorts, longs and
   ulongs, one element each, as .byte -1, .byte 255, .short -3,
   .quad -1 and .quad 18446744073709551615, signed or unsigned by the
   element's type; names as .quad .Ls0 and .quad .Ls1; pointers as
   .quad n, .quad a+8, .quad on_signal and .quad 0; months, of unknown
   size, as .size months, 8, .string "Jan" and .string "Feb"; letters
   as .byte 74, .byte 97, .byte 110, .zero 1 and .string "Feb"; words,
   whose first row the literal fills, as .ascii "abc" and .zero 3;
   scalar as .long 5; string as .quad .Ls2; zero, of {}, as .zero 4 in
   .data; empty as .zero 12; then .bss, n and a.  An array of 16 bytes
   or more is under .p2align 4 on x86-64, which the psABI asks of a
   global array, and under its element's alignment on aarch64. */
int n;
int a[4];
void on_signal(int);

int counted[] = { 1, 2, 3 };
int padded[5] = { 1, 2 };
int table[2][3] = { { 1, 2, 3 }, { 4, 5, 6 } };
int partly[2][3] = { { 1 }, 4, 5 };
int rows[][3] = { 1, 2, 3, 4 };
signed char narrow[] = { -1 };
unsigned char bytes[] = { 255 };
short shorts[] = { -3 };
long longs[] = { -1 };
unsigned long ulongs[] = { -1 };
char *names[] = { "Jan", "Feb" };
int *pointers[] = { &n, a + 2, (int *) on_signal, 0 };
char months[][4] = { "Jan", "Feb" };
char letters[2][4] = { { 'J', 'a', 'n' }, "Feb" };
char words[2][3] = { "abc" };
int scalar = { 5 };
char *string = { "abc" };
int zero = { };
int empty[3] = { };
