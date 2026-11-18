/* An enumeration costs nothing.  An enumerated type is the integer type
   compatible with it, and an enumeration constant is its number
   (shape.md chapters 3 and 5), so an object, a parameter, a result, a
   pointer, an array, a cast, a sizeof, a switch and the arithmetic
   written through one compile to what they compile to with the integer
   type and the number spelled out.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program with unsigned int for enum color, int
   for enum delta and long for enum wide, and 0, 1 and 2 for red, green
   and blue, -1 and 1 for down and up, and -2 and 4294967296 for low and
   high.

   On aarch64 the listing must be the same twin's listing, byte for byte
   but for the .file line. */
enum color { red, green, blue };
enum delta { down = -1, up = 1 };
enum wide { low = -2, high = 0x100000000 };

enum color
next(enum color c)
{
        switch (c) {
        case red: return green;
        case green: return blue;
        default: return red;
        }
}

int
signs(enum color c, enum delta d)
{
        return (c - 1 < 0) + (d < down + up) * 2 + (int) sizeof(enum wide);
}

long
widen(enum wide w, enum color *p)
{
        enum color a[2];

        a[0] = *p;
        a[1] = (enum color) w;
        return w + a[0] + a[1] + high + low;
}
