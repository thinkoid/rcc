/* Wide string literals and wide character constants, 6.4.4.5 and
   6.4.5, which the encoding prefixes L, u and U make possible.  word is
   an array of wchar_t that L"crème brûlée" initializes, 6.7.11p16, an
   element a code point, the source's UTF-8 decoded.  length() counts a
   wide string's elements to its zero, L'\0', and the C library's
   wcslen() counts them again.  The program counts the elements past
   ASCII, compares two of them with L'è' and with the code point of
   L'é', and checks the sizes: of the array, of a wide constant and of
   a wide literal two adjacent literals make, one of them of no prefix,
   6.4.5p5; of a char16_t literal whose 😀 takes a surrogate pair; the
   code point of U'😀'; and the two UTF-8 bytes the universal character
   name \u00e9 gives a literal of no prefix, 6.4.3p4.

   It writes the line "12 12 3 52" and exits 123, the length times ten
   and the three accented letters, or exits 200 plus the number of the
   first check that failed.

   Under rcc and under clang -O2 it prints the line and exits 123:
   nothing here is undefined.

   On x86-64 and on aarch64 the listing must show word's data as its
   thirteen elements, the zero last, under one .long, aligned as an
   array of its 52 bytes is, .p2align 4 on x86-64 and .p2align 2 on
   aarch64; smile's as .short 233, 55357, 56832, 0 after .p2align 1;
   and the literal L"ab" "c" as .long 97, 98, 99, 0 under a label of
   its own after .p2align 2. */
#include <stdio.h>
#include <wchar.h>

static wchar_t word[] = L"crème brûlée";
static unsigned short smile[] = u"é😀";

/* the elements of s before its zero */
static int
length(const wchar_t *s)
{
        int n = 0;

        while (s[n] != L'\0')
                n++;

        return n;
}

int
main(void)
{
        const wchar_t *joined = L"ab"
                                "c";
        int accents = 0;
        int i;

        for (i = 0; word[i] != L'\0'; i++)
                if (word[i] > 0x7f)
                        accents++;

        printf("%d %d %d %d\n", length(word), (int) wcslen(word), accents,
               (int) sizeof word);

        if (length(word) != 12 || wcslen(word) != 12 || accents != 3)
                return 201;
        if (word[2] != L'è' || word[10] != 0xe9 || L'é' != 0xe9)
                return 202;
        if (sizeof word != 13 * sizeof(wchar_t) || sizeof L'x' != 4)
                return 203;
        if (sizeof(L"ab"
                   "c") != 16 ||
            length(joined) != 3 || joined[2] != 'c')
                return 204;
        if (sizeof smile != 8 || smile[1] != 0xd83d || smile[2] != 0xde00)
                return 205;
        if (U'😀' != 0x1f600 || sizeof "\u00e9" != 3)
                return 206;

        return length(word) * 10 + accents;
}
