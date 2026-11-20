/* backend.md 6.3 and chapter 10 and aarch64.md 6.3 and chapter 10, an
   array at file scope, by a variable and by a constant index.  By a
   variable, the array decays to its address, PC-relative for table,
   which the file defines, and from the GOT for other, which it only
   declares, an array of unknown size; then the subscript's access, as
   for a local array, but for aarch64's label row, whose access holds
   table's offset in its page.  By a constant, table's element is its
   label plus the offset, the address by a name and a constant, and no
   instruction forms the array's address first; other's is the GOT's
   load and the offset as the displacement of the access through it.

   On x86-64 the listing must show get() and put() forming the address
   with leaq table(%rip), %rax and accessing (%rax,%rcx,4) after
   movslq -4(%rbp), %rcx, the index's load and widening; third() the
   one movl table+12(%rip), %eax; get_other() the same as get() with
   movq other@GOTPCREL(%rip), %rax in place of the leaq, and
   third_other() that movq and movl 12(%rax), %eax.  table is in
   .bss, .size table, 32, .p2align 4, the psABI's 16 for a global array
   of 16 bytes or more, and .zero 32.

   On aarch64 the listing must show get() and put() taking the label's
   row, adrp x0, table, add x0, x0, w1, sxtw #2 and the access at
   [x0, :lo12:table]; third() the element's page and its load, both
   with the offset, adrp x16, table+12 and
   ldr w0, [x16, :lo12:table+12]; get_other() the GOT's load, adrp x0,
   :got:other and ldr x0, [x0, :got_lo12:other], and the access at
   [x0, w1, sxtw #2], and third_other() those two and ldr w0, [x0,
   #12].  table is in .bss under its element's .p2align 2. */
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
