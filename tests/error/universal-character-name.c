/* A universal character name in a string literal, which is not yet.
   clang -E writes some of them out as UTF-8; this one it passes
   through as written. */
int main(void)
{
    return sizeof "\U0001F600";
}
