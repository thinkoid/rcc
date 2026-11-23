/* A call through a pointer to a function whose parameter a marking
   attribute marks, cast to a pointer to the unmarked function type:
   the call would pass the parameter as an int where the attribute asks
   for another size, so the cast is unsupported, by the attribute's
   words, as the call through the marked type is. */
int
call(int (*p)(int x __attribute__((mode(word)))))
{
        return ((int (*)(int)) p)(1);
}

int
main(void)
{
        return 0;
}
