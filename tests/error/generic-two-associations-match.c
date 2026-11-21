/* A controlling expression compatible with two associations' types: a
   pointer to an array of unknown size is compatible with a pointer to
   an array of three and with one of four, 6.7.7.3p6, which are not
   compatible with each other, and the type shall be compatible with at
   most one, 6.5.2.1p2. */
int (*any)[];

int
main(void)
{
        return _Generic(any, int (*)[3]: 0, int (*)[4]: 1);
}
