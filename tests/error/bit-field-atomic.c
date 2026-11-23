/* A bit-field of an atomic type, J.3.10(3): the specifier is dropped
   with a warning, the field is an int bit-field, and the record is
   laid out as with int, two ints. */
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
