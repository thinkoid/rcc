/* A static inline function defined twice in a system header.  The
   line marker says what clang -E says of an #include of a system
   header: the definitions are the text of one, header.h, by the flag
   3. */
# 1 "header.h" 1 3
static inline int
one(void)
{
        return 1;
}

static inline int
one(void)
{
        return 1;
}
