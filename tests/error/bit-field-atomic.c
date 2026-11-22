/* A bit-field of an atomic type, which rcc does not use yet: the record
   is not laid out, and its size is refused by the type's words, as for
   a member of the type. */
struct counter
{
        _Atomic(int) count : 3;
        int other;
};

int
main(void)
{
        return sizeof(struct counter);
}
