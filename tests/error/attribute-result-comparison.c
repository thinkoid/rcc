/* Two pointers to functions compared, one whose result is of a typedef
   through a marking attribute and one whose result is long: rcc keeps
   nothing of what mode (word) makes of long, so it cannot tell whether
   the two function types are compatible, and the comparison is
   unsupported, by the attribute's words, as the conversion is. */
typedef long word_t __attribute__((mode(word)));

int
same(word_t (*p)(void), long (*q)(void))
{
        return p == q;
}

int
main(void)
{
        return 0;
}
