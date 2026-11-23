/* The same struct in the program's own source: the member rcc refuses
   as not yet is refused where it stands. */
struct counter
{
        _Alignas(8) int count;
};

int
main(void)
{
        return sizeof(struct counter);
}
