/* A static struct in a block whose member is initialized by a local's
   value, which is no constant expression, 6.7.11p5: an error. */
struct point
{
        int x, y;
};

int
main(void)
{
        int n = 1;
        static struct point p = { n, 0 };

        return p.x;
}
