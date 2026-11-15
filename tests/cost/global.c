/* backend.md 6.3, 6.6 and chapter 10 and aarch64.md 6.3 and chapter 10,
   a global the file defines: the object is reached PC-relative, a slot
   with another spelling.  get() reads it, set() writes it, add() adds
   to it, step() steps it, and address() returns its address.

   On x86-64 the listing must show get() reading it with movl
   counter(%rip), %eax; set() storing with movl %eax, counter(%rip);
   add() and step() operating on the object, the memory forms of 6.6,
   addl %eax, counter(%rip) and addl $1, counter(%rip); and address()
   forming its address with leaq counter(%rip), %rax.  The object is in
   .bss, under .globl counter, .type counter, @object, .size counter, 4
   and .p2align 2, its label and .zero 4.

   On aarch64 the listing must show each access as two instructions,
   adrp x16, counter and the load or the store through
   [x16, :lo12:counter]: get() ldr w0; set() str w0; add() v first,
   then the load, add w1, w1, w0 and the store, each access with its own
   adrp, since the machine has no memory form; step() the same with
   add w0, w0, #1; and address() adrp x0, counter and
   add x0, x0, :lo12:counter.  The object is in .bss as on x86-64. */
int counter;

int
get(void)
{
        return counter;
}

void
set(int v)
{
        counter = v;
}

void
add(int v)
{
        counter += v;
}

void
step(void)
{
        counter++;
}

int *
address(void)
{
        return &counter;
}
