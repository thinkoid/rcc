/* shape.md chapter 5: a local array of char initialized by a string
   literal, 6.7.11p15, alone or in braces: the literal's bytes copied
   from its own object, under its own label with its zero (G17), into
   the array by the copy of a record's assignment (G18), in units of
   the element, 1 byte, lowest address first, as many as the literal
   has with its zero or the array holds, the fewer; then the bytes past
   them zeroed, after the copy, by the zeroing of a list, each byte of
   the array written once (G22).  exact() is char s[4] = "abc", four
   bytes; unsized() an array of unknown size, which takes the 17 bytes
   of "a longer message", past eight units; padded() char u[100] =
   "abc", four bytes copied and 96 zeroed; braced() char v[] = {
   "abc" }, which is exact()'s; and hundred() an array of unknown size
   of 100 bytes, a string of 99 characters and its zero.

   On x86-64 each unit is the load of a byte into a scratch register
   and its store, the unit's offset in each place's displacement:
   exact() movzbl .Ls0(%rip), %eax; movb %al, -4(%rbp) and the same at
   .Ls0+1 to .Ls0+3 and -3 to -1, eight instructions; unsized() the
   literal's address first, leaq .Ls1(%rip), %rax, then the loop of one
   index, movq $-17, %rdx, then movzbl 17(%rax,%rdx,1), %ecx; movb %cl,
   (%rbp,%rdx,1); addq $1, %rdx; jne back, six instructions; padded()
   exact()'s eight at -100 to -97, then the zeroing's loop, movq $-96,
   %rax, then movb $0, (%rbp,%rax,1); addq $1, %rax; jne back; braced()
   exact()'s; and hundred() unsized()'s six with movq $-100, %rdx and
   100(%rax,%rdx,1).

   On aarch64 each unit is the load of a byte into x17 and its store:
   exact() the literal's address formed once, adrp x0, .Ls0; add x0,
   x0, :lo12:.Ls0, then ldrb w17, [x0]; strb w17, [x29, #16] and the
   same at #1 to #3 and #17 to #19, ten instructions; unsized() the
   slot's address and the literal's, add x0, x29, #16; adrp x1, .Ls1;
   add x1, x1, :lo12:.Ls1, the count, mov x16, #17, then the loop of
   four, ldrb w17, [x1], #1; strb w17, [x0], #1; subs x16, x16, #1;
   b.ne back, eight instructions; padded() exact()'s ten at #16 to #19,
   then the zeroing's loop, add x0, x29, #20; mov x16, #96, then strb
   wzr, [x0], #1; subs x16, x16, #1; b.ne back; braced() exact()'s; and
   hundred() unsized()'s eight with mov x16, #100.  On both targets
   each literal is a .string under a label of its own in .data. */
void use(void *);

void
exact(void)
{
        char s[4] = "abc";

        use(s);
}

void
unsized(void)
{
        char t[] = "a longer message";

        use(t);
}

void
padded(void)
{
        char u[100] = "abc";

        use(u);
}

void
braced(void)
{
        char v[] = { "abc" };

        use(v);
}

void
hundred(void)
{
        char h[] = "0123456789012345678901234567890123456789"
                   "0123456789012345678901234567890123456789"
                   "0123456789012345678";

        use(h);
}
