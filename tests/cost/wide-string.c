/* backend.md and aarch64.md chapter 10, the data of wide string
   literals, 6.4.5p6: each its own array in .data, after the string
   literals' block opens, aligned to its element, its elements at their
   width under one directive, the zero element last; an array of
   wchar_t or of char16_t's type that a wide literal initializes,
   6.7.11p16, its elements and its zero, and the elements past them
   zero, p22.  greeting() returns L"hé", joined() U"é" "!", two
   adjacent literals of one prefix, 6.4.5p5, second() reads an element
   of L"xyz" by a constant index, and wide_constants() adds three wide
   constants, which fold.

   On x86-64 the listing must show L"hé" as .p2align 2; .Ls0:;
   .long 104, 233, 0, and U"é" "!" as .long 233, 33, 0 under .Ls1;
   padded as .long 97, 98, 0 and .zero 20 after .p2align 4, an array of
   32 bytes; code_units as .short 120, 55357, 56832, 0 after .p2align
   1, the surrogate pair of U+1F600; second() reading the element by
   movl .Ls2+4(%rip), %eax; and wide_constants() as movq $128865, %rax.

   On aarch64 the listing must show the same literals' data, and padded
   after .p2align 2, its element's alignment; second() reading the
   element by adrp x16, .Ls2+4; ldr w0, [x16, :lo12:.Ls2+4], whose
   scaled offset the literal's alignment to 4 makes valid; and
   wide_constants() as mov x0, #63329; movk x0, #0x1, lsl #16. */
#include <stddef.h>

wchar_t padded[8] = L"ab";
unsigned short code_units[] = u"x😀";

const wchar_t *
greeting(void)
{
        return L"hé";
}

const unsigned int *
joined(void)
{
        return U"é"
               "!";
}

long
wide_constants(void)
{
        return L'é' + u'x' + U'😀';
}

int
second(void)
{
        return L"xyz"[1];
}
