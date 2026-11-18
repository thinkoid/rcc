/* A struct passed by value to a function declared with a struct
   parameter: the declaration is accepted, and the call is not yet. */
struct point
{
        int x, y;
};

int first(struct point p);

int
call(struct point *p)
{
        return first(*p);
}

int
main(void)
{
        return 0;
}
