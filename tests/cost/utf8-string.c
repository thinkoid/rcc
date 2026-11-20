/* choices.md J.3.5(4), (6) and (8): a string literal of no prefix
   keeps the source's bytes, and the source is UTF-8, so the literal
   "é" is the two bytes of its UTF-8, 0xc3 and 0xa9, and a zero.  s is
   the array of char that it initializes, 6.7.11p15, and first()
   reads its first byte as plain char, the target's: -61 on x86-64,
   195 on aarch64.

   On x86-64 and on aarch64 the listing must show s as .size s, 3 and
   .string "\303\251", the bytes in octal; first() reading s[0] with
   movsbl on x86-64, plain char being signed there, and with ldrb on
   aarch64, where it is unsigned. */
char s[] = "é";

int
first(void)
{
        return s[0];
}
