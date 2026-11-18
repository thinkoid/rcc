/* An array of wchar_t initialized by a wide string literal longer than
   the array, 6.7.11p3. */
#include <stddef.h>

wchar_t name[2] = L"wide";

int
main(void)
{
        return 0;
}
