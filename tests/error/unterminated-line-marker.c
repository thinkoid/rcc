/* A line marker whose file name has no closing quote.  A macro that
   expands to '#' at the start of a line makes clang -E write the
   marker, which rcc's lexer then reads; clang warns of the open quote
   first. */
#define HASH #
HASH 1 "error.c

    int
    main(void)
{
        return 0;
}
