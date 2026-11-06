/* G6: pointer arithmetic is address arithmetic. n is 2^61 - 1, all
   ones shifted right by argc + 2, so p + n adds 8n = 2^64 - 8 to the
   address and wraps to p - 8; p < p + n is false and the exit status
   0. clang -O2 marks the add inbounds and folds p < p + n to n > 0
   (probe 2), which is true: 1. argv is an object the optimizer cannot
   size, and the shift count comes through argc, so n is unknown until
   the fold has applied. */
int main(int argc, char **argv)
{
    char **p = argv;
    unsigned long ones = -1;
    long n = ones >> (argc + 2);

    return p < p + n;
}
