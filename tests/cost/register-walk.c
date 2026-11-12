/* aarch64.md 2.3 and 6.3, and backend.md chapter 4: *p++ and *--q on
   a register pointer, in the string idioms of the register style, a
   copy, a length and a compare.

   On x86-64 each step is the row of p++ on a register variable, the
   read copied and the variable stepped in place, and then the access
   through the copy: movq %rbx, %rax; addq $1, %rbx; movsbl (%rax),
   %eax.  The listing must show the copy and the step twice in copy()'s
   loop, for p, whose address comes first, and then for q, before the
   load through q's copy and movb %cl, (%rax); the row once in
   length()'s; for q++ and for p++ in compare()'s, and *--q as subq $1,
   %r12 and the load through (%r12).

   On aarch64 each access steps its pointer itself, by the
   post-indexed form for p++ and q++ and the pre-indexed one for --q,
   and no step is an instruction of its own.  The listing must show
   ldrb w0, [x20], #1 and strb w0, [x19], #1 in copy()'s loop, the two
   ends of *p++ = *q++, and then cbz w0; ldrb w0, [x19], #1 and cbz w0
   in length()'s; ldrb w0, [x19] and ldrb w1, [x20], #1 in compare()'s,
   then ldrb w0, [x19], #1 for *p++, and ldrb w1, [x20, #-1]! for *--q
   after it.  No add or sub writes x19 or x20, and no mov copies either
   but copy()'s start = p, mov x21, x19. */
char *copy(register char *p, register char *q)
{
    register char *start = p;
    while (*p++ = *q++)
        ;
    return start;
}

int length(register char *p)
{
    register int n = 0;
    while (*p++)
        n++;
    return n;
}

int compare(register char *p, register char *q)
{
    while (*p == *q++)
        if (*p++ == 0)
            return 0;
    return *p - *--q;
}
