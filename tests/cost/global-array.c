/* backend.md 6.3 and chapter 10 and aarch64.md 6.3 and chapter 10, an
   array at file scope, by a variable and by a constant index: the
   array decays to its address, PC-relative for table, which the file
   defines, and from the GOT for other, which it only declares, an array
   of unknown size; then the subscript's access, as for a local array.

   On x86-64 the listing must show get() and put() forming the address
   with leaq table(%rip), %rax and accessing (%rax,%rcx,4) after
   movslq %ecx, %rcx; third() the leaq and movl 12(%rax), %eax;
   get_other() and third_other() the same with
   movq other@GOTPCREL(%rip), %rax in place of the leaq.  table is in
   .bss, .size table, 32, .p2align 4, the psABI's 16 for a global array
   of 16 bytes or more, and .zero 32.

   On aarch64 the listing must show get() and put() forming the address
   with adrp x0, table and add x0, x0, :lo12:table and accessing
   [x0, w1, sxtw #2]; third() the pair and ldr w0, [x0, #12];
   get_other() and third_other() the same with adrp x0, :got:other and
   ldr x0, [x0, :got_lo12:other] in place of the pair.  table is in
   .bss under its element's .p2align 2. */
int table[8];
extern int other[];

int
get(int i)
{
        return table[i];
}

int
third(void)
{
        return table[3];
}

void
put(int i, int v)
{
        table[i] = v;
}

int
get_other(int i)
{
        return other[i];
}

int
third_other(void)
{
        return other[3];
}
