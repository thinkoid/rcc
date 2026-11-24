/* A function declared again with a pointer to a pointer to char where
   its first declaration has a pointer to a typedef through a marking
   attribute made from int: whatever mode (word) makes of int, it is no
   pointer, so the two pointers are not compatible, 6.7.7.2p2, nor are
   the two function types, 6.7.7.4p14, as without the pointers, and the
   declarations conflict. */
typedef int word_t __attribute__((mode(word)));

void f(word_t *w);
void f(char **w);

int
main(void)
{
        return 0;
}
