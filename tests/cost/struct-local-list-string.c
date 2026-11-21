/* shape.md chapter 5: a string literal for an array member of char in
   a local record's list, 6.7.11p15: the list's rule as for any value
   (G20), the whole record zeroed first, then each value stored at its
   place in the list's order, and the string's place takes the copy of
   the literal from its own object (G17) by the copy of a record's
   assignment (G18), in units of the element, 1 byte, as many as the
   literal has with its zero or the array holds; the bytes of the array
   past the copy are the zeroing's, with no second zeroing (G22).
   exact() is struct name one = { "abc", 1 }, of a member of four
   chars, which the literal fills; longer() the same list for a member
   of twelve, eight of them left to the zeroing.

   On x86-64 the zeroing is the store of the immediate 0 in units of
   the record's alignment, then the copy is each byte loaded into a
   scratch register and stored, then the int stored: exact() movl $0,
   -8(%rbp); movl $0, -4(%rbp), then movzbl .Ls0(%rip), %eax; movb %al,
   -8(%rbp) and the same at .Ls0+1 to .Ls0+3 and -7 to -5, then movl
   $1, -4(%rbp); longer() four movl $0 at -16 to -4, then the same
   copy at -16 to -13, then movl $1, -4(%rbp), and nothing at -12 to
   -5.

   On aarch64 the zeroing is the store of the zero register in units of
   the record's alignment, then the copy is each byte loaded into x17
   from the literal's address, formed once, and stored, then the int
   stored: exact() str wzr, [x29, #16]; str wzr, [x29, #20], then adrp
   x0, .Ls0; add x0, x0, :lo12:.Ls0; ldrb w17, [x0]; strb w17, [x29,
   #16] and the same at #1 to #3 and #17 to #19, then mov w0, #1; str
   w0, [x29, #20]; longer() four str wzr at #16 to #28, then the same
   copy at #16 to #19, then mov w0, #1; str w0, [x29, #28], and nothing
   at #20 to #27. */
struct name
{
        char text[4];
        int count;
};

struct label
{
        char text[12];
        int count;
};

void use(void *);

void
exact(void)
{
        struct name one = { "abc", 1 };

        use(&one);
}

void
longer(void)
{
        struct label one = { "abc", 1 };

        use(&one);
}
