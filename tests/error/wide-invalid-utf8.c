/* A wide string literal whose text is no UTF-8: the byte 0xff leads no
   character, and the elements of a wide literal are code points. */
int
main(void)
{
        return sizeof(L"cafÿ");
}
