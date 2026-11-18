/* An assignment to a member of an array type, which is no modifiable
   lvalue, 6.3.2.1p1. */
struct name
{
        char text[8];
};

void
clear(struct name *n, char *other)
{
        n->text = other;
}

int
main(void)
{
        return 0;
}
