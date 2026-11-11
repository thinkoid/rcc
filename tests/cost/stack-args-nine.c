/* backend.md chapter 8 and aarch64.md chapter 8, arguments past the
   convention's registers: each argument is stored to its slot as it
   is evaluated, and those the registers do not take are placed on the
   stack from their slots, the first of them at the lowest address.
   nine() passes nine arguments, the ninth a char, and ten() ten ints.

   On x86-64 the listing must show the arguments past the sixth pushed
   from their slots last first, each loaded into eax and pushed from
   rax, the char with movsbl -33(%rbp), %eax; nine() with three on the
   stack over an 8-byte pad, subq $8, %rsp, and ten() with four and no
   pad; and addq $32, %rsp after each call.

   On aarch64 the listing must show each constant argument stored
   through w0, mov w0, #1 and str w0, [x29, #16], the char with strb;
   after the stores, the outgoing area opened with sub sp, sp, #16 in
   both functions, nine()'s 8 bytes rounded up to 16; each argument
   past the eighth copied from its slot through x17 at its own width,
   ldrb w17, [x29, #48] and strb w17, [sp] for the char, ldr and str
   w17 at [sp] and [sp, #8] for ten()'s two; then the first eight
   loaded into w0 to w7, the bl, and add sp, sp, #16. */
int f9(int, int, int, int, int, int, int, int, char);
int f10(int, int, int, int, int, int, int, int, int, int);

int nine(void)
{
    return f9(1, 2, 3, 4, 5, 6, 7, 8, 9);
}

int ten(void)
{
    return f10(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
}
