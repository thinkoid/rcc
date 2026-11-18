/* An assignment of a struct that is incomplete, which has no object to
   copy, 6.3.2.1p1: an error. */
extern struct opaque left, right;

int
main(void)
{
        left = right;

        return 0;
}
