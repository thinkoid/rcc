/* An object of a struct type declared at file scope and defined
   elsewhere: the declaration is accepted, and so is the object's
   address, which needs no layout; its value, which c1 does not read
   yet, is not yet. */
struct point
{
        int x, y;
};

extern struct point origin;

int
main(void)
{
        origin;
        return 0;
}
