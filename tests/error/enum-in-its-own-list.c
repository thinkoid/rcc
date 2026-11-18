/* An enumeration named in its own enumerator list, where it is
   incomplete, 6.7.3.4p5: rcc takes no use of it there. */
enum color { red, green, count = sizeof(enum color) };

int
main(void)
{
        return 0;
}
