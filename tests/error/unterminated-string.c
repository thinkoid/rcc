/* A string literal with no closing quote on its line; clang -E warns
   of it first. */
/* clang-format off */
int
main(void)
{
        return sizeof "abc;
}
