/* A function declared again with long where its first declaration has
   a typedef through a marking attribute made from long: rcc keeps
   nothing of what mode (word) makes of long, so it cannot tell whether
   the two parameter types are compatible, and the redeclaration is
   unsupported, by the attribute's words. */
typedef long word_t __attribute__((mode(word)));

void f(word_t w);
void f(long w);

int
main(void)
{
        return 0;
}
