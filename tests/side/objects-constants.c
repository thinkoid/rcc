/* The initializer of an object at file scope is an integer constant
   expression, 6.7.11p4, which the builders fold with rcc's arithmetic,
   that of the machine: a shift count masked, a division truncated, a
   constant of a wider type converted to int by its low bits. */
int k_and = 1 && 0;
int k_bits = (12 & 10) | (12 ^ 10);
int k_char = 'a';
int k_cmp = 3 < 4;
int k_div = -7 / 2;
int k_hex = 0x7f;
int k_lognot = !5;
int k_long = 2147483648;
int k_max = 2147483647;
int k_min = -2147483647 - 1;
int k_neg = -1;
int k_neglong = -2147483648;
int k_not = ~0;
int k_octal = 017;
int k_or = 0 || 7;
int k_ovf_and = 0 && 2147483647 + 1;
int k_paren = (((5)));
int k_rem = -7 % 2;
int k_shl31 = 1 << 31;
int k_shl32 = 1 << 32;
int k_shr = -16 >> 2;
int k_sum = 1 + 2 * 3;
int k_unsigned = 0x80000000;
int k_wide = 4294967297;
int k_plus = +5;
int k_cast = (int) (unsigned char) 300 + (short) 70000;
