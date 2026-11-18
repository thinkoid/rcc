/* A struct's list with more initializers than the struct has members,
   which would initialize an object the struct does not contain,
   6.7.11p3: an error. */
struct point
{
        int x, y;
};

struct point corner = { 1, 2, 3 };

int
main(void)
{
        return corner.x;
}
