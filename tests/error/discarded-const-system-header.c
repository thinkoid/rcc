/* A pointer to char initialized from a pointer to const char in a
   system header's text, as a header's macro writes it: the header is
   read as it is, so the conversion draws no warning there, and the
   program compiles.  The line markers say what clang -E says of an
   #include of a system header: the function is the text of one,
   header.h, by the flag 3, and the program's own text follows it. */
# 1 "header.h" 1 3
static const char name[] = "rcc";

char *
writable(void)
{
        char *p = name;
        return p;
}
# 15 __BASE_FILE__ 2

int
main(void)
{
        return 0;
}
