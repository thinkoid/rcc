/* An enumeration constant one past the greatest unsigned long: no
   integer type holds it, and the enumeration has no type for all its
   values, 6.7.3.3p12 and its footnote. */
enum mask { all = 0xffffffffffffffff, beyond };

int
main(void)
{
        return 0;
}
