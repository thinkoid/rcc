/* A typedef declared again with the integer type of the enumeration it
   names: the two are compatible, 6.7.3.3p13, and not the same type,
   which 6.7.1p4 asks of a typedef name declared again. */
enum color { red, green };
typedef enum color hue;
typedef unsigned int hue;

int
main(void)
{
        return 0;
}
