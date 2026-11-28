/* A value reached through a signed overflow is warned of, 6.6p4. */
enum e { A = 0x7fffffff + 1 };

int
f(void)
{
        return A;
}
