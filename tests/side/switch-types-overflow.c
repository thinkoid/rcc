/* A case constant reached through a signed overflow on a switch of
   another type than int, 6.6p4: rcc warns at its first token, of an int
   constant on a char and on a long switch and of a long one on a long
   switch, and the case is the value wrapped; an unsigned wrap is no
   overflow. */
int
w_ovf_c(void)
{
        char x;
        switch (x) {
        case 2147483647 + 1: return 1;
        case 0: return 2;
        }
        return 0;
}

int
w_ovf_i_on_l(void)
{
        long x;
        switch (x) {
        case 2147483647 + 1: return 1;
        case 0: return 2;
        }
        return 0;
}

int
w_ovf_l(void)
{
        long x;
        switch (x) {
        case 0x7fffffffffffffff + 1: return 1;
        case 0: return 2;
        }
        return 0;
}

int
w_noovf_u(void)
{
        unsigned x;
        switch (x) {
        case 0u - 1: return 1;
        case 0: return 2;
        }
        return 0;
}
