/* A function declared again with a pointer to long where its first
   declaration has a pointer to a typedef through a marking attribute
   made from int: mode (word) makes an integer of int, and rcc keeps
   nothing of which, so it cannot tell whether that integer is long,
   and the redeclaration is unsupported, by the attribute's words. */
typedef int word_t __attribute__((mode(word)));

void f(word_t *w);
void f(long *w);

int
main(void)
{
        return 0;
}
