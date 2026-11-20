/* A line marker whose number does not fit an int.  A macro that
   expands to '#' at the start of a line makes clang -E write the
   marker, which rcc's lexer then reads. */
#define HASH #
HASH 99999999999 "error.c"

    int
    main(void)
{
        return 0;
}
