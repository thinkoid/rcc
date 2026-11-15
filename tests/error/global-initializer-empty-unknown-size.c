/* The empty initializer for an array of unknown size, which gives it
   no element: an array of unknown size shall not be initialized by an
   empty initializer, 6.7.11p4. */
int table[] = { };

int
main(void)
{
        return 0;
}
