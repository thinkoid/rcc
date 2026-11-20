/* choices.md J.3.4(2): every character of an identifier is
   significant, 6.4.2.1p13, and an object's symbol is its whole name.
   The two objects of file scope have names of 68 characters that
   differ only in the last, past the 31 significant initial characters
   of an external identifier that 5.2.5.2 sets as the least; the two
   locals of sum() have names of 68 that differ in the last, past the
   63 of an internal one.  Each pair is two objects, and sum() adds the
   four.

   On x86-64 the listing must show each object's name whole, under its
   own .globl, .type, .size and label, the two differing in their last
   letter, a and b; sum() reading each by its whole name, movl
   significant_characters_of_an_external_identifier_counted_beyond_63_a(%rip)
   and the same of _b; and the two locals in two slots, movl $1,
   -4(%rbp) and movl $2, -8(%rbp).

   On aarch64 the listing must show the same names, sum() reading each
   by adrp x16 and ldr of :lo12: and its whole name, and the two locals
   in two slots, str w0, [x29, #16] and str w0, [x29, #20]. */
int significant_characters_of_an_external_identifier_counted_beyond_63_a;
int significant_characters_of_an_external_identifier_counted_beyond_63_b;

int
sum(void)
{
        int significant_characters_of_an_internal_identifier_counted_beyond_63_c =
            1;
        int significant_characters_of_an_internal_identifier_counted_beyond_63_d =
            2;

        return significant_characters_of_an_external_identifier_counted_beyond_63_a +
               significant_characters_of_an_external_identifier_counted_beyond_63_b +
               significant_characters_of_an_internal_identifier_counted_beyond_63_c +
               significant_characters_of_an_internal_identifier_counted_beyond_63_d;
}
