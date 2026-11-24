/* Two pointers to functions compared by <, one whose result is of a
   typedef through a marking attribute and one whose result is long:
   whatever mode (word) makes of long, a relational operator asks two
   pointers to object types, 6.5.9p2, and a function is none, so the
   comparison is an error, as for two pointers to one function type,
   and not a refusal by the attribute's words. */
typedef long word_t __attribute__((mode(word)));

int
before(word_t (*p)(void), long (*q)(void))
{
        return p < q;
}

int
main(void)
{
        return 0;
}
