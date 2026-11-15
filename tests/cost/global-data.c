/* backend.md chapter 10 and aarch64.md chapter 10, the data of the
   objects a file defines, after the last function: those with an
   initializer in .data, those without in .bss, each in the order of
   its definition, under .globl, .type and .size, its alignment when it
   is above 1, its label and its value.  The alignment is the type's,
   but for an array of 16 bytes or more on x86-64, which the psABI
   aligns to 16.  An initializer is
   an integer constant expression folded and converted to the object's
   type, or a null pointer.  zero is in .data, since the source wrote
   its initializer; n is defined tentatively twice, one object in .bss;
   later is declared, then defined, and takes the place of its
   definition.

   On x86-64 and on aarch64 the listing is the same text but for one
   line: .data, then c as .byte 120, s as .p2align 1 and .short -3, i as
   .p2align 2 and .long 42, l as .p2align 3 and .quad 4886718345, ones
   as .quad 18446744073709551615, null as .quad 0, zero as .long 0 and
   later as .long 7; then .bss, n as .zero 4 and table, of 12 bytes, as
   .zero 12, each under .p2align 2, and pair, of 16 bytes, as .zero 16,
   under .p2align 4 on x86-64 and under its element's .p2align 3 on
   aarch64. */
char c = 'x';
short s = -3;
int i = 40 + 2;
long l = 0x123456789;
unsigned long ones = -1;
int *null = 0;
int zero = 0;
int n;
int n;
int table[3];
long pair[2];
extern int later;
int later = 7;
