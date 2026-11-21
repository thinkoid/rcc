/* A pointer to one enumerated type assigned to a pointer to another,
   both of the integer type unsigned int: each enumeration is a type of
   its own, 6.2.5p21, compatible with its integer type alone,
   6.7.3.3p13, so 6.5.17.2p1 asks a cast. */
enum color { red, green };
enum shape { round, square };

int
main(void)
{
        enum color c = green;
        enum color *p = &c;
        enum shape *q = p;

        return 0;
}
