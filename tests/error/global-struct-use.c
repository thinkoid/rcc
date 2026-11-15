/* An object of a struct type declared at file scope and defined
   elsewhere: the declaration is accepted, and an expression that
   names it, which needs the object, is not yet. */
extern struct point origin;

int
main(void)
{
        return &origin != 0;
}
