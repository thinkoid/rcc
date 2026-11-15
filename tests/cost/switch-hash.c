/* backend.md 7.3 and aarch64.md 7.3, the hash form.  The cases are
   twelve status codes, 200 to 503: ncase is 12, ten or more, and range,
   503 less 200, is 303, above 3 * 12 = 36.  The table's size is the i
   from 12 / 4 = 3 to 12 / 2 = 6 whose i * worst is least, worst the
   most cases of one remainder modulo i:

   - i = 3: the remainders hold 2, 4 and 6 cases, worst 6, i * worst 18;
   - i = 4: 6, 3, 1 and 2, worst 6, 24;
   - i = 5: 3, 3, 1, 2 and 3, worst 3, 15;
   - i = 6: 1, 2, 4, 1, 2 and 2, worst 4, 24.

   So the size is 5.  The value is divided unsigned by 5, the jump goes
   through a table of five buckets by the remainder, and each bucket
   compares the quotient with its cases' values divided by 5, in the
   order of the values: remainder 0 holds 200, 400 and 500, quotients
   40, 80 and 100; 1 holds 201, 301 and 401, 40, 60 and 80; 2 holds 302,
   60; 3 holds 403 and 503, 80 and 100; 4 holds 204, 304 and 404, 40, 60
   and 80.

   On x86-64 the listing must show movl $5, %ecx, xorl %edx, %edx and
   divl %ecx, the quotient in eax and the remainder in edx;
   leaq .Lreason.16(%rip), %rcx and jmp *(%rcx,%rdx,8), with no
   movl %edx, %edx before them, since divl wrote edx at 32 bits; the
   table, five .quad of the buckets .Lreason.17 to .Lreason.21; and in
   each bucket
   cmpl $40, %eax and the rest, each with its je, and jmp .Lreason.14 to
   the default.

   On aarch64 the listing must show mov w1, #5, udiv w2, w0, w1 and
   msub w0, w2, w1, w0, the quotient in w2 and the remainder in w0; the
   table's address in x16, ldr x16, [x16, w0, uxtw #3] and br x16; the
   same table; and in each bucket cmp w2, #40 and the rest, each with
   its b.eq, and b .Lreason.14. */
int
reason(int code)
{
        switch (code) {
        case 200: return 1;
        case 201: return 2;
        case 204: return 3;
        case 301: return 4;
        case 302: return 5;
        case 304: return 6;
        case 400: return 7;
        case 401: return 8;
        case 403: return 9;
        case 404: return 10;
        case 500: return 11;
        case 503: return 12;
        default: return 0;
        }
}
