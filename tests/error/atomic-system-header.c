/* _Atomic in a system header's text, as stdatomic.h writes it: the
   qualifier and the specifier are dropped with no warning, and the
   program, which reads and steps the objects as ints, compiles with no
   word.  The line markers say what clang -E says of an #include of a
   system header: the declarations are the text of one, header.h, by
   the flag 3, and the program's own text follows it. */
# 1 "header.h" 1 3
typedef _Atomic(int) atomic_int;

struct counter
{
        _Atomic int count;
        int *_Atomic next;
};
# 16 __BASE_FILE__ 2

int
main(void)
{
        atomic_int n = 0;
        struct counter c = { 0, 0 };

        n++;
        return n + c.count - 1;
}
