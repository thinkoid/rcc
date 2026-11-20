/* backend.md chapter 10 and aarch64.md chapter 10, the data of an
   array of char that a string literal in parentheses initializes: the
   parentheses are the literal, 6.5.2p6, so the array takes it,
   6.7.11p15, without braces and in a list alike, as global-string's
   whole and braced do.

   On x86-64 and on aarch64 the listing is the same text: under .data,
   bare and listed each under .size 4 as .string "abc", and no literal
   of its own. */
char bare[] = ("abc");
char listed[] = { ("abc") };
