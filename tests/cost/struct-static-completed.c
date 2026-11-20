/* shape.md chapter 3 and backend.md chapter 10, a tentative definition
   of internal linkage of an object of a struct that the unit completes
   after it: the object takes the struct as it is at the end of the
   unit, 6.9.3p2, as one of external linkage does, and is made as any
   object of the struct.

   On x86-64 and on aarch64 the listing must show origin in .bss with
   no .globl, under .size origin, 8, .p2align 2 and .zero 8, and y_of
   reading origin.y at origin+4: movl origin+4(%rip), %eax on x86-64,
   ldr w0, [x16, :lo12:origin+4] on aarch64. */
static struct point origin;

struct point
{
        int x, y;
};

int
y_of(void)
{
        return origin.y;
}
