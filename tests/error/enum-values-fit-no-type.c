/* An enumeration of a negative value and one past LONG_MAX: no integer
   type holds both, 6.7.3.3p4. */
enum span { below = -1, above = 0xffffffffffffffff };

int
main(void)
{
        return 0;
}
