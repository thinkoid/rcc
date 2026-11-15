/* A static function called before its definition, which is static
   inline: rcc does not compile the definition, so the call, which
   would need it, is not yet, at the end of the unit. */
static int helper(int x);

int
main(void)
{
        return helper(1);
}

static inline int
helper(int x)
{
        return x + 1;
}
