/* A conditional whose arms are a pointer to a function whose parameter
   a marking attribute marks and a pointer to the unmarked function
   type: equality does not read the mark, so the arms join at the first
   arm's type, and a call through the result would pass the parameter
   as an int where the attribute asks for another size.  The join is
   not yet, by the attribute's words, as the call through the marked
   type is. */
int
call(int c, int (*q)(int), int (*p)(int x __attribute__((mode(word)))))
{
        return (c ? q : p)(1);
}

int
main(void)
{
        return 0;
}
