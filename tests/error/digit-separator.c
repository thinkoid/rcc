/* C23's digit separator, 1'000 (6.4.4.2), is not in the dialect rcc
   preprocesses in, C17 with GNU's extensions: the ' opens a character
   constant, which the line does not close. */
int
main(void)
{
        return 1'000 != 1000;
}
