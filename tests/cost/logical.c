/* backend.md 6.3 and aarch64.md 6.3, a && b and a || b as a value: the
   jump code of chapter 7 into one register, 1 or 0 written on the arms.
   Each operand is tested against 0, and the right one is not loaded
   when the left decides.

   On x86-64 the listing must show each operand compared with
   cmpl $0, %eax, and no setcc: both() jumping with je to movl $0, %eax
   when a or b is 0, and otherwise writing movl $1, %eax; and either()
   jumping with jne past b's test to movl $1, %eax when a is not 0, and
   b's test jumping with je to movl $0, %eax.

   On aarch64 the listing must show each operand tested with cbz or
   cbnz, and no cset: both() branching with cbz w0 to mov w0, #0 when a
   or b is 0, and otherwise writing mov w0, #1; and either() branching
   with cbnz w0 past b's test to mov w0, #1 when a is not 0, and b's
   test branching with cbz w0 to mov w0, #0. */
int both(int a, int b)
{
    return a && b;
}

int either(int a, int b)
{
    return a || b;
}
