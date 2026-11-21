/* A pointer to a function whose parameter a marking attribute marks,
   converted to a pointer to the unmarked function type: equality does
   not read the mark, so the two types are one to rcc, and a call
   through the converted pointer would pass the parameter as an int
   where the attribute asks for another size.  The conversion is not
   yet, by the attribute's words, as the call through the marked type
   is. */
int
call(int (*p)(int x __attribute__((mode(word)))))
{
        int (*q)(int) = p;

        return q(1);
}

int
main(void)
{
        return 0;
}
