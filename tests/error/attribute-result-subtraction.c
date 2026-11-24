/* Two pointers to functions subtracted, one whose result is of a
   typedef through a marking attribute and one whose result is long:
   whatever mode (word) makes of long, subtraction asks two pointers to
   complete object types, 6.5.7p3, and a function is none, so the
   subtraction is an error, as for two pointers to one function type,
   and not a refusal by the attribute's words. */
typedef long word_t __attribute__((mode(word)));

long
apart(word_t (*p)(void), long (*q)(void))
{
        return p - q;
}

int
main(void)
{
        return 0;
}
