/* A string literal with no closing quote on its line; clang -E warns
   of it first. */
int main(void)
{
    return sizeof "abc;
}
