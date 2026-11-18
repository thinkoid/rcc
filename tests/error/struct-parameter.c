/* A function defined with a struct parameter, which passes the struct
   by value: not yet. */
struct point
{
        int x, y;
};

int
first(struct point p)
{
        return 0;
}

int
main(void)
{
        return 0;
}
