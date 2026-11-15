/* backend.md 6.3 and chapter 10 and aarch64.md 6.3 and chapter 10, a
   global the file only declares, which may be in a shared library: its
   address is loaded from the GOT, and each access is the indirection
   through it, as *p is with p loaded.  get(), set(), add(), step() and
   address() as in global, and increment(), whose two accesses load the
   address twice, the place first.

   On x86-64 the listing must show each function loading the address
   with movq counter@GOTPCREL(%rip), %rax; get() then movl (%rax), %eax;
   set() and add() loading v after it and storing with
   movl %ecx, (%rax) or adding with addl %ecx, (%rax); step()
   addl $1, (%rax); address() the load alone; increment() the address
   for the store into rax, then the address for the load into rcx,
   movl (%rcx), %ecx, addl $1, %ecx and movl %ecx, (%rax).  Nothing is
   written for the object.

   On aarch64 the listing must show each address loaded in two
   instructions, adrp x0, :got:counter and
   ldr x0, [x0, :got_lo12:counter]; get() then ldr w0, [x0]; set()
   str w1, [x0]; add() ldr w2, [x0], add w2, w2, w1 and str w2, [x0];
   step() the same with #1; address() the load alone; increment() the
   address for the store into x0, then the one for the load into x1,
   ldr w1, [x1], the add and str w1, [x0]. */
extern int counter;

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

void
increment(void)
{
        counter = counter + 1;
}
