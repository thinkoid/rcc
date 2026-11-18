/* Adjacent string literals of two prefixes, which 6.4.5p2 forbids. */
int
main(void)
{
        return sizeof(L"wide"
                      u"utf-16");
}
