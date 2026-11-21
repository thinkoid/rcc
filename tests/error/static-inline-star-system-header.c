/* '*' for the size of the array a parameter of a system header's
   static inline definition is adjusted from: a variable length array
   of unspecified size is no part of a parameter declaration in a
   function definition, 6.9.2p6, a header's included, though its body
   is skipped.  The line marker says what clang -E says of an #include
   of a system header: the definition is the text of one, header.h, by
   the flag 3. */
# 1 "header.h" 1 3
static inline int
first(int n, int a[*])
{
        return a[0];
}
