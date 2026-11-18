/* A string literal of no prefix whose text is no UTF-8, concatenated
   with a wide one: its elements are read as code points too, 6.4.5p5. */
int
main(void)
{
        return sizeof("cafÿ"
                      L"!");
}
