/* An enumeration a marking attribute marks: packed makes the type as
   small as its values allow, and rcc does not honour it, so an object
   of the type is not yet.  Its constants are ints, and usable. */
enum __attribute__((packed)) color { red, green };

int
main(void)
{
        enum color c = green;

        return c;
}
