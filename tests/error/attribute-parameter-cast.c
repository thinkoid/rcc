/* A pointer to a function whose parameter a marking attribute marks,
   cast to a pointer to the unmarked function type and stored: a call
   through the stored pointer would pass the parameter as an int where
   the attribute asks for another size, so the cast is unsupported, by
   the attribute's words, called or not. */
int (*plain)(int);

void
keep(int (*p)(int x __attribute__((mode(word)))))
{
        plain = (int (*)(int)) p;
}

int
main(void)
{
        return 0;
}
