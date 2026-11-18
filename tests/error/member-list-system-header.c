/* A struct a system header defines with a member rcc refuses as not
   yet, an _Atomic one: by the rule for a system header the struct is a
   not-yet record of the refusal's words and the header parses on, and
   the size of the struct, which needs its layout, is refused by those
   words.  The line markers say what clang -E says of an #include of a
   system header: the struct is the text of one, header.h, by the flag
   3, and the program's own text follows it. */
# 1 "header.h" 1 3
struct counter
{
        _Atomic int count;
};
# 14 __BASE_FILE__ 2

int
main(void)
{
        return sizeof(struct counter);
}
