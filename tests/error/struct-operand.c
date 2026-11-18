/* A struct as an operand of +, which takes two arithmetic operands or
   a pointer and an integer, 6.5.7p2: an error. */
struct point
{
        int x, y;
};

struct point origin;

int
main(void)
{
        return origin + 1;
}
