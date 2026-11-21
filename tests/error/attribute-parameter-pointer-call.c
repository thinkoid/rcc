/* A call through a pointer to a function whose parameter a marking
   attribute marks: the function type carries the attribute's words, so
   the call, which would pass the parameter as an int where the
   attribute asks for another size, is not yet, as a call of a function
   the attribute marks is. */
int
call(int (*p)(int x __attribute__((mode(word)))))
{
        return p(1);
}

int
main(void)
{
        return 0;
}
