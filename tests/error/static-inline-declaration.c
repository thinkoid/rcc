/* A static inline function declared without a body in a system
   header: only the definition of a system header's static inline
   function is taken.  The line markers say what clang -E says of an
   #include of a system header: the declaration is the text of one,
   header.h, by the flag 3, and the program's own text follows it. */
# 1 "header.h" 1 3
static inline int twice(int x);
# 9 __BASE_FILE__ 2

int
main(void)
{
        return 0;
}
